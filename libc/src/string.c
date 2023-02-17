#include "../include/string.h"

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

int strncmp (const char* s1, const char* s2, size_t n)
{
	if (n-- == 0) return 0;

 	while ((*s1 != '\0') && (*s2 != '\0') && (*s1 == *s2) && n--)
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
