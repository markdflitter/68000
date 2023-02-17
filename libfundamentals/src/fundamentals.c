#include "../include/fundamentals.h"

void* __memset (void* ptr, int value, size_t num)
{
	unsigned char* p = (unsigned char*) ptr;

	while (num--)
		*p++ = (unsigned char) value;

	return p;
}

void* __memcpy (void* destination, const void* source, size_t num)
{
	unsigned char* d = (unsigned char*) destination;
	const unsigned char* s = (unsigned char*) source;

	while (num--)
		*d++ = *s++;

	return destination;
}
