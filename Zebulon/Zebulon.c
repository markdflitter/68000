#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define DISPLAY 0xE00001
void setDisplay (char n)
{
	char* p = (char*) DISPLAY;
	*p = n; 
}

#define TXA 0xC00007
#define SRA 0xC00003
#define TXREADY 0x4
void putch (char c)
{
	volatile char* sra = (char*) SRA;
	volatile char* txa = (char*) TXA;

	while ((*sra & TXREADY) == 0x0)
		;

	*txa = c;
}

void mfputs (char* s)
{
	while (*s != 0)
	{
		putch (*s++);
	}
}

#define RXA 0xC00007
#define RXREADY 0x1
char getch ()
{
	volatile char* sra = (char*) SRA;
	volatile char* rxa = (char*) RXA;

	while ((*sra & RXREADY) == 0x0)
		;

	return *rxa;
}

void mfgets (char* buf, int limit, int* length)
{
	for (;;)
	{
		char c = getch ();
		putch (c);

		if (*length == limit)
			return ;

		if (c == '\r')
			return ;
		
		(*length)++;
		*buf++ = c;
	}
}

void printhelp ()
{
	mfputs ("exit\t - exit to monitor\n\r");
	mfputs ("version\t - print version\n\r");
	mfputs ("help\t - print this help\n\r");
}

void main (void)
{
	char* version = "Zebulon V1.0\n\r";

	mfputs (version);
	mfputs ("type help for help\r\n");

	char buf [21];
	int length;
	int exit = FALSE;
	while (!exit)
	{
		length = 0;
		mfputs ("$ ");
		mfgets (buf, 20, &length);
		buf [length] = '\0';
		mfputs ("\n\r");

		if (strcmp (buf, "exit")) exit = TRUE;
		if (strcmp (buf, "version")) puts (version);
		if (strcmp (buf, "help")) printhelp();
	}
}

