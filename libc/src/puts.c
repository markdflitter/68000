#include "../include/stdio.h"
#include <ZebulonAPI.h>

int putchar (int c)
{
	Zebulon::_zebulon_putch (c);
	return 0;
}

int puts (const char* s)
{
	while (*s != 0)
	{
		putchar (*s++);
	}
	return 0;
}

