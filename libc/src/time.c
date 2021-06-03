#include "../include/time.h"
#include <Zebulon.h>

clock_t clock()
{
	return Zebulon::time ();
}

