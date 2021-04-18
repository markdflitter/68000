#include "../include/stdio.h"
#include "../include/libgloss.h"

int puts (char* s)
{
	while (*s != 0)
	{
		__putch (*s++);
	}
}

