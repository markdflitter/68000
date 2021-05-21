#include "../include/stdlib.h"
#include "../include/ctype.h"

extern char* __end;

void* malloc (size_t size)
{
	if (size & 0x1)	size = size + 1;

		static char* top_of_heap = (char*) &__end;

	char* alloc = top_of_heap;
		
	top_of_heap += size;

	return (void*) alloc;
}

void free (void* ptr)
{
}

void abort (void)
{
 	asm ("move.l 4,%a0");
 	asm ("jmp (%a0)");

	while (1)
		;
}

int atoi (const char* str)
{
	bool negate = false;

	const char* c = str;
	while (*c != '\0' && !isdigit (*c))
		if (*c++ == '-') negate = !negate;

	int result = 0;
	while (*c != '\0')
	{
		if (isdigit (*c))
		{
			result *= 10;
			result += (*c - '0');
		}
		c++;
	}

	return negate ? -result : result;
}


long atol (const char* str)
{
	bool negate = false;

	const char* c = str;
	while (*c != '\0' && !isdigit (*c))
		if (*c++ == '-') negate = !negate;

	long result = 0;
	while (*c != '\0')
	{
		if (isdigit (*c))
		{
			result *= 10;
			result += (*c - '0');
		}
		c++;
	}

	return negate ? -result : result;
}



