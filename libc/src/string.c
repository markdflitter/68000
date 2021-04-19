#include "../include/string.h"

 int strlen (const char* s)
 {
	int result = 0;

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
