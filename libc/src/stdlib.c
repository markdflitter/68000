#include "../include/stdlib.h"

extern char* __end;

void* malloc (size_t size)
{
	static char* top_of_heap = (char*) 1;
	if (top_of_heap == (char*) 1)
	{
      top_of_heap = (char*) &__end;
	}

	char* alloc = top_of_heap;
		
	top_of_heap += size;

	return (void*) alloc;
}

void free (void* ptr)
{
}


