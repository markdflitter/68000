#include <size_t.h>
#include <stdlib.h>

void * operator new (size_t n)
{
	return malloc (n);
}

