#ifndef ZEBULONAPI_H
#define ZEBULONAPI_H

// a lot of stuff in this file is technically not volatile, but marking it sit so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it refences the wrong stack location

#include "TrapHelper.h"

namespace
{
	unsigned int trap (unsigned char trap)
  	{
		volatile unsigned int result;
		setA0 (&result);
		
		call_trap (trap);

		return result;
	}

	void trap (unsigned char trap, unsigned char opcode, int p1)
  	{
		setOpcode (opcode);

		setA0 (&p1);	

		call_trap (trap);
	}

	int trap (unsigned char trap, unsigned char opcode)
  	{
		setOpcode (opcode);

		int result;	
		setA0 (&result);	

		call_trap (trap);

		return result;
	}
}


namespace Zebulon
{
	enum TRAP {time = 0, IO = 1};
	enum IO {write_char = 1, read_char = 2};

inline unsigned int _zebulon_time ()
{
	return trap (time);
}

inline void _zebulon_putch (int c)
{
	trap (IO, write_char, c);
}

inline int _zebulon_getch ()
{
	return trap (IO, read_char);
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

