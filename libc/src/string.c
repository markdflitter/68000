#include "../include/string.h"

 int strlen (char* s)
 {
	int result = 0;

 	while (*s++ != '\0')
 		result++;

	return result;
}


int strcmp (char* s1, char* s2)
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
