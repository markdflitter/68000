#include <stdio.h>
#include <string.h>

void printhelp ()
{
	puts ("exit\t - exit to monitor\n\r");
	puts ("version\t - print version\n\r");
	puts ("help\t - print this help\n\r");
}

void main (void)
{
	char* version = "Zebulon V1.1\n\r";

	int i = 0x12345678;
	printhex (i);

	int j = 0xABCDEFAB;
	printhex (j);


	printf ("%d",i,j);

	return ;

	puts (version);
	puts ("type help for help\r\n");

	char buf [21];
	int length;
	int exit = 0;
	while (!exit)
	{
		length = 0;
		puts ("$ ");
		char* p = gets (buf);
		*p = '\0';
		puts ("\n\r");

		if (strcmp (buf, "exit") == 0) exit = 1;
		if (strcmp (buf, "version") == 0) puts (version);
		if (strcmp (buf, "help") == 0) printhelp();
	}
}

