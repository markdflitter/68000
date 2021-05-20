#include "../include/stdlib.h"

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
