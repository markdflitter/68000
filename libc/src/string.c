#include "../include/string.h"

void* memset (void* ptr, int value, size_t num)
{
	unsigned char* p = (unsigned char*) ptr;

	while (num--)
		*p++ = (unsigned char) value;

	return p;
}

void* memcpy (void* destination, const void* source, size_t num)
{
	unsigned char* d = (unsigned char*) destination;
	const unsigned char* s = (unsigned char*) source;

	while (num--)
		*d++ = *s++;

	return destination;
}

size_t strlen (const char* s)
{
	size_t result = 0;

 	while (*s++ != '\0')
 		result++;

	return result;
}

int strcmp (const char* s1, const char* s2)
{
 	while ((*s1 != '\0') && (*s2 != '\0') && (*s1 == *s2))
	{
		s1++; s2++;
	}

	if (*s1 == *s2)
	{
		return 0;
	}
	else
	{
		return *s1 < *s2 ? -1 : 1;
	}
}
