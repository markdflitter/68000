#ifndef ZEBULONAPI_H
#define ZEBULONAPI_H

#include "TrapHelper.h"

// a lot of stuff in this file is technically not volatile, but marking it so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it references the wrong stack location

namespace
{
	inline volatile unsigned int trap (unsigned char trap)
  	{
		volatile unsigned int result;
		setA0 (&result);
		
		call_trap (trap);

		return result;
	}

	inline volatile void trap (unsigned char trap, unsigned char opcode, int p1)
  	{
		setOpcode (opcode);

		setA0 (&p1);	

		call_trap (trap);
	}

	inline volatile int trap (unsigned char trap, unsigned char opcode)
  	{
		setOpcode (opcode);

		volatile int result;	
		setA0 (&result);	

		call_trap (trap);

		return result;
	}

	inline volatile unsigned int trap (unsigned int trap, unsigned char opcode, unsigned long p1, unsigned char* data)
	{
		setOpcode (opcode);
		setP1 (p1);

		volatile int result;	
		setA0 (&result);	

		setA1 (data);

		call_trap (trap);	
	
		return result;	
	}
}


namespace Zebulon
{
	enum TRAP {trap_time = 0, trap_IO = 1, trap_block = 2};
	enum IO {IO_write_char = 1, IO_read_char = 2};
	enum block {block_write_block = 1, block_read_block = 2};

inline unsigned int _zebulon_time ()
{
	return trap (trap_time);
}

inline void _zebulon_putch (int c)
{
	trap (trap_IO, IO_write_char, c);
}

inline int _zebulon_getch ()
{
	int result = trap (trap_IO, IO_read_char);
	return result;
}

enum ide_result {IDE_OK = 0x0, IDE_AMNF = 0x1, IDE_TK0NF = 0x2, IDE_ABRT = 0x4, IDE_MCR = 0x8,
				IDE_IDNF = 0x10, IDE_MC = 0x20, IDE_UNC = 0x40, IDE_BBK = 0x80};

inline unsigned int _zebulon_read_block (unsigned long block, unsigned char data [512])
{
	return trap (trap_block, block_read_block, block, data);
}

inline unsigned int _zebulon_write_block (unsigned long block, unsigned char data [512])
{
	return trap (trap_block, block_write_block, block, data);
}

}
#endif

