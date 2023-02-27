#include "../include/string.h"
#include <fundamentals.h>

void* memset (void* ptr, int value, size_t num)
{	
	return __memset (ptr, value, num);
}

void* memcpy (void* destination, const void* source, size_t num)
{
	return __memcpy (destination, source, num);
}
