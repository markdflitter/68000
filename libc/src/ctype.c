#include "../include/ctype.h"

int isdigit (int arg)
{
	return arg >= '0' && arg <= '9';
}

int isprint (int arg)
{
	return arg >= ' ' && arg <= 126;
}

