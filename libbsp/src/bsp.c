#include <libgloss.h>

#define TXA 0xC00007
#define SRA 0xC00003
#define TXREADY 0x4
void __putch (char c)
{
	volatile char* sra = (char*) SRA;
	volatile char* txa = (char*) TXA;
	while ((*sra & TXREADY) == 0x0)
		;

	*txa = c;
}


#define RXA 0xC00007
#define RXREADY 0x1
char __getch ()
{
	volatile char* sra = (char*) SRA;
	volatile char* rxa = (char*) RXA;

	while ((*sra & RXREADY) == 0x0)
		;
	
	return *rxa;
}
