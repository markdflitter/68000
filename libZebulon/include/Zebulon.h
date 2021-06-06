#ifndef ZEBULON_H
#define ZEBULON_H

namespace Zebulon
{

inline unsigned int _zebulon_time ()
{
	volatile unsigned int result;
	volatile unsigned int* p = &result;

	asm ("movel %0,%%a0\n\t"
		 "trap #0\n\t" : : "m" (p) : "a0");

	return result;
}

inline void _zebulon_putch (char c)
{
	volatile char cc = c;
	volatile char* p = &cc;

	asm ("moveb #1,%%d0\n\t"
		 "movel %0,%%a0\n\t"
		 "trap #1\n\t" : : "m" (p) : "a0","d0");
}

inline char _zebulon_getch ()
{
	volatile char result;
	volatile char* p = &result;

	asm ("moveb #2,%%d0\n\t"
		 "movel %0,%%a0\n\t"
		 "trap #1\n\t" : : "m" (p) : "a0","d0");

	return result;
}	

}

#endif

