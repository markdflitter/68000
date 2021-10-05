#ifndef ZEBULONAPI_H
#define ZEBULONAPI_H

// a lot of stuff in this file is technically not volatile, but marking it sit so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it refences the wrong stack location

#include "TrapHelper.h"

namespace
{
	enum TRAP {time = 0};

	unsigned int trap (unsigned char trap)
  	{
		volatile unsigned int result;
		setResultAddress (&result);
		
		call_trap (trap);

		return result;
	}
}


namespace Zebulon
{

inline unsigned int _zebulon_time ()
{
	return trap (time);
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

enum ide_result {IDE_OK = 0x0, IDE_AMNF = 0x1, IDE_TK0NF = 0x2, IDE_ABRT = 0x4, IDE_MCR = 0x8,
				IDE_IDNF = 0x10, IDE_MC = 0x20, IDE_UNC = 0x40, IDE_BBK = 0x80};

inline unsigned int _zebulon_read_block (unsigned long block, unsigned char data [512])
{
	volatile unsigned long d1 = block;
	volatile void* a0 = data;
	unsigned int result;
	volatile void* a1 = &result;

	asm ("moveb #1, %%d0\n\t"
		 "movel %0, %%d1\n\t"
		 "movel %1, %%a0\n\t"
		 "movel %2, %%a1\n\t"
		 "trap #2\n\t" : : "m" (d1), "m" (a0), "m" (a1) : "d0", "d1", "a0", "a1");

	return result;
}

inline unsigned int _zebulon_write_block (unsigned long block, unsigned char data [512])
{
	volatile unsigned long d1 = block;
	volatile void* a0 = data;
	unsigned int result;
	volatile void* a1 = &result;

	asm ("moveb #2, %%d0\n\t"
		 "movel %0, %%d1\n\t"
		 "movel %1, %%a0\n\t"
		 "movel %2, %%a1\n\t"
		 "trap #2\n\t" : : "m" (d1), "m" (a0), "m" (a1) : "d0", "d1", "a0", "a1");

	return result;
}

}
#endif

