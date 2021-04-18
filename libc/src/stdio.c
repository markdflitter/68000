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
