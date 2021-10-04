#include "../include/time.h"
#include <ZebulonAPI.h>

clock_t clock()
{
	return Zebulon::_zebulon_time ();
}

