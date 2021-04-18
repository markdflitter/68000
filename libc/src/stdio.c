#include "../include/stdio.h"
#include "../include/libgloss.h"

int puts (char* s)
{
	while (*s != 0)
	{
		__putch (*s++);
	}
}


char* gets (char* s)
{
	char* buf = s;
	for (;;)
	{
 		char c = __getch ();
		__putch (c);

 		if (c == '\r')
 			return buf;

 		*buf++ = c;
 	}
 
	return buf;
}

