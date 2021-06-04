#ifndef ZEBULON_H
#define ZEBULON_H

#include <stdio.h>

namespace Zebulon
{

unsigned int time ()
{
	unsigned int result;
	register unsigned int* pResult asm("a0") = &result;

	asm ("trap #0" : : "a" (pResult));

	return result;
}

}

#endif

