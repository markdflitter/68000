#ifndef ZEBULON_H
#define ZEBULON_H

#include <stdio.h>

namespace Zebulon
{

unsigned int time ()
{
	unsigned int time;
	asm ("trap #0\n\t" : : "a" (&time));

	return time;
}

}

#endif

