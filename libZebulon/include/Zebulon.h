#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{

inline unsigned int _zebulon_time ()
{
	unsigned int result;
	register unsigned int* pResult asm("a0") = &result;

	asm ("trap #0" : : "a" (pResult));

	return result;
}

inline void _zebulon_putch (char c)
{
	register char d0 asm("d0") = c;

	asm ("trap #1" : : "d" (d0));
}

}

#endif

