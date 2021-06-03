#ifndef ZEBULON_H
#define ZEBULON_H

#include <stdio.h>

namespace Zebulon
{

unsigned int time ()
{
	volatile unsigned int time;
	register volatile unsigned int* a0 asm ("a0") = &time;
	asm ("trap #0\n\t" : : "r" (a0));

	return time;
}

}

#endif

