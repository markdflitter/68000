#include "../include/stdlib.h"
#include "../include/ctype.h"
#include "../include/stdio.h"

extern char __end;

// 65K is yer lot
#define MAX_HEAP_SIZE 0x10000

char* initialiseHeap ()
{
	return &__end;
}

static char* base_of_heap = initialiseHeap ();
static char* top_of_heap = base_of_heap;
static char* heap_limit = base_of_heap + MAX_HEAP_SIZE;

void* malloc (size_t size)
{
	if (size & 0x1)	size = size + 1;

//printf ("top_of_heap 0x%x\n\r", top_of_heap);

	char* alloc = top_of_heap;
		
	top_of_heap += size;

	if (top_of_heap - (char*) &__end > MAX_HEAP_SIZE)
	{
		printf (">>> heap full - aborting\n\r");
		abort ();
		//printf (">> heapsize 0x%x - time to consider implementing delete\n\r",top_of_heap - (char*) &__end);
	}

	//printf ("malloc %d 0x%x\n\r", size, alloc);

	return (void*) alloc;
}

void free (void* ptr)
{
	//printf ("free 0x%x\n\r", ptr);
}


void heap_diag ()
{
	printf ("HEAP: base 0x%x, top 0x%x, limit 0x%x - used %d / %d (%d%%)\n\r", base_of_heap, top_of_heap, heap_limit, (top_of_heap - base_of_heap), MAX_HEAP_SIZE, 100 * (top_of_heap - base_of_heap) / MAX_HEAP_SIZE);
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



