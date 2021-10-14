#include "../include/stdlib.h"
#include "../include/ctype.h"
#include "../include/stdio.h"

extern char __end;

// 65K is yer lot
#define MAX_HEAP_SIZE 0x10000

namespace
{

char* initialiseHeap ()
{
	return &__end;
}

unsigned int zero ()
{
	return 0;
}

template <class T>
T max (const T& t1, const T& t2) { return t1 > t2 ? t1 : t2; }

size_t roundUp (size_t input)
{
	for (size_t level = 1; level < MAX_HEAP_SIZE; level *= 2)
		if (input <= level) return level;
}

}

static char* base_of_heap = initialiseHeap ();
static char* top_of_heap = base_of_heap;
static char* heap_limit = base_of_heap + MAX_HEAP_SIZE;

static unsigned int malloc_count = zero ();
static unsigned int free_count = zero ();



void* malloc (size_t requestedSize)
{
	//add 4 bytes for length, min size 8, round up to next power of two
	size_t allocSize = roundUp (max (requestedSize + 4, 8UL));

	char* alloc = top_of_heap;
		
	top_of_heap += allocSize;

	if (top_of_heap - (char*) &__end > MAX_HEAP_SIZE)
	{
		printf (">>> heap full - aborting\n\r");
		abort ();
		//printf (">> heapsize 0x%x - time to consider implementing delete\n\r",top_of_heap - (char*) &__end);
	}

	malloc_count++;
	printf ("[%d] malloc %d -> %d 0x%x\n\r", malloc_count, requestedSize, allocSize, alloc);
	
	return (void*) alloc;
}

void free (void* ptr)
{
	free_count++;
	printf ("[%d] free 0x%x\n\r", free_count, ptr);
}


void heap_diag ()
{
	printf ("HEAP: base 0x%x, top 0x%x, limit 0x%x - used %d / %d (%d%%) [%d malloc'd - %d freed = %d allocated]\n\r", base_of_heap, top_of_heap, heap_limit, (top_of_heap - base_of_heap), MAX_HEAP_SIZE, 100 * (top_of_heap - base_of_heap) / MAX_HEAP_SIZE, malloc_count, free_count, malloc_count - free_count);
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



