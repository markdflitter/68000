#include "../include/stdlib.h"
#include "../include/ctype.h"
#include "../include/stdio.h"

extern char* __end;

void* malloc (size_t size)
{
	if (size & 0x1)	size = size + 1;

	static char* top_of_heap = (char*) &__end;
	//printf ("top_of_heap 0x%x\n\r", top_of_heap);

	char* alloc = top_of_heap;
		
	top_of_heap += size;

	if (top_of_heap - (char*) &__end > 0x40000)
		printf (">> heapsize 0x%x - time to consider implementing delete\n\r",top_of_heap - (char*) &__end);

	//printf ("malloc %d 0x%x\n\r", size, alloc);

	return (void*) alloc;
}

void free (void* ptr)
{
	//printf ("free 0x%x\n\r", ptr);
}

void abort (void)
{
 	asm ("movel #4, %a0");
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



