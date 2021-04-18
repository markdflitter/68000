#include "../include/stdio.h"
#include "../include/libgloss.h"

int getchar(void)
{
	return  __getch ();
}

char* gets (char* s)
{
	char* buf = s;
	for (;;)
	{
 		char c = __getch ();
		__putch (c);

 		if (c == '\r')
 			break;

 		*buf++ = c;
 	}
 
	return buf;
}


int putchar (int c)
{
	__putch ((char) c);
}


int puts (const char* s)
{
	while (*s != 0)
	{
		__putch (*s++);
	}
}



void printhex (int i)
{
	int j = i;

 	char result [9];
  	const char* LUT = "0123456789ABCDEF";

	result [8] = '\0';

	for (int l = 0; l < 8; l++)
	{
		result [7 - l] = LUT [j & 0xF];
		j = j >> 4;
	}

	puts (result);
}



int printf (const char* format, ...)
{	
	va_list ap;
	va_start (ap, format);

	int a = (int) &format;
	printhex (a);

	printhex ((int) ap);

	int i = va_arg (ap, int);
	printhex (i);

	int j = va_arg (ap, int);
	printhex (j);
}

