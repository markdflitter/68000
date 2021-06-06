#include "../include/cxa_atexit.h"

dso_handle __dso_handle = 0;

extern "C" int __cxa_atexit (void (*func) (void*), void* arg, void* dso_handle)
{
	return 0;
}

