#include <stdio.h>

int main (void)
{
	asm volatile ("moveb #9,0xE00001");
	puts ("HelloWorld\n\r");
}

