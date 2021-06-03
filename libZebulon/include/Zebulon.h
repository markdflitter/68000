#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{

int time ()
{
	unsigned int time = 0;
	unsigned int* atime = &time;
	asm ("move.l %0, %%a0\n\t"
		 "trap #0\n\t" : "=m" (atime));

	return time;
}

}

#endif

