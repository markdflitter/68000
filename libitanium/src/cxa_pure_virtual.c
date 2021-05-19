#include "../include/cxa_pure_virtual.h"
#include <stdio.h>

void __cxa_pure_virtual ()
{
	printf ("pure virtual function called\n\r");

	for (;;)
		;
}
