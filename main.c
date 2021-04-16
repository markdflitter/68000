
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

void print (char* s)
{
	while (*s != 0)
	{
		putch (*s++);
	}
}


void main (void)
{
	print ("HelloWorld\r\n");
}

