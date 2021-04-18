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
	if (strlen (s1) != strlen (s2))
		return 0;
 
 	while (*s1 != '\0')
 	{
 		if (*s1 != *s2)
 			return 0;
 		s1++;
 		s2++;
	}

	return 1;
}



