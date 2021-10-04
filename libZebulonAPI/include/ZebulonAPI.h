#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{
// a lot of stuff in this file is technically not volatile, but marking it sit so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it refences the wrong stack location
	
inline unsigned int _zebulon_time ()
{
	volatile unsigned int result;
	volatile void* a0 = &result;

	asm ("movel %0, %%a0\n\t"
		 "trap #0\n\t" : : "m" (a0) : "a0");

	return result;
}

inline void _zebulon_putch (int c)
{
	volatile int cc = c;	// although this is technically not volatile, it forces the compiler to make a local copy of the parameter which defeats a compiler bug where it refences the wrong stack location
	volatile void* a0 = &cc;

	asm ("moveb #1, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "trap #1\n\t" : : "m" (a0) : "d0", "a0");
}

inline int _zebulon_getch ()
{
	volatile int result;
	volatile void* a0 = &result;

	asm ("moveb #2, %%d0\n\t"
		 "movel %0, %%a0\n\t"
		 "trap #1\n\t" : : "m" (a0) : "d0", "a0");

	return result;
}	

}
#endif

