#include "../include/cxa_guard.h"

int __cxa_guard_acquire (cxa_guard* g)
{
	int result = !(*g);
	*g = 1;
	return result;
}

void  __cxa_guard_release (cxa_guard* g)
{

}

void  __cxa_guard_abort (cxa_guard* g)
{

}
