#include <stdio.h>

int main (void)
{
	asm volatile ("moveb #9,0xE00001");
	char* p = new char;
	*p = 'x';
	puts ("HelloWorld\n\r");
	delete p;
}

