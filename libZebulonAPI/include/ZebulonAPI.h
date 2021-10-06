#ifndef ZEBULONAPI_H
#define ZEBULONAPI_H

#include "TrapHelper.h"
#include <stdio.h>

// a lot of stuff in this file is technically not volatile, but marking it so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it references the wrong stack location

namespace
{
/*
	inline volatile unsigned int trapR (unsigned char trap_num, unsigned char opcode)
  	{
		setOpcode (opcode);

		volatile unsigned int result;
		volatile unsigned int* pResult = &result;
		setOpcode (opcode);

	asm volatile ("movel %0, %%a0\n\t" : : "m" (pResult) : "a0");
		//setResultPtr (pResult);
		
		//if (trap_num == 2) printf ("rd opcode %d\n\r", getOpcode ());		
		call_trap (trap_num);

		return result;
	}

	inline volatile unsigned int trapR (unsigned int trap_num, unsigned char opcode, unsigned long p1, unsigned char* data)
	{
		printf ("%d\n\r", opcode);		
		setP1 (p1);
		setA1 (data);

		return trapR (trap_num, opcode);	
	}

	inline void trap (unsigned char trap_num)
  	{
		call_trap (trap_num);
	}

	inline void trap (unsigned char trap_num, unsigned char opcode)
  	{
		setOpcode (opcode);
		trap (trap_num);
	}

	inline void trap (unsigned char trap_num, unsigned char opcode, unsigned int p1)
  	{
		setP1 (p1);
		trap (trap_num, opcode);
	}
*/
}


namespace Zebulon
{
	enum TRAP {trap_time = 0, trap_serialIO = 1, trap_ide = 2};
	enum serialIO {serialIO_write_char = 1, serialIO_read_char = 2};
//	enum ide_operations {ide_write_block = 1, ide_read_block = 2};

inline unsigned int _zebulon_time ()
{
	unsigned int result;
	trap (trap_time, &result);
	return result;
}

inline void _zebulon_putch (int c)
{
	trap (trap_serialIO, serialIO_write_char, &c);
}

inline int _zebulon_getch ()
{
	int result;
	trap (trap_serialIO, serialIO_read_char, &result);
	return result;
}

enum ide_result {IDE_OK = 0x0, IDE_AMNF = 0x1, IDE_TK0NF = 0x2, IDE_ABRT = 0x4, IDE_MCR = 0x8,
				IDE_IDNF = 0x10, IDE_MC = 0x20, IDE_UNC = 0x40, IDE_BBK = 0x80};

inline unsigned int _zebulon_read_block (unsigned long block, unsigned char data [512])
{
/*
	printf ("readblock\n\r");
	return trapR (trap_ide, ide_read_block, block, data);
*/
	return 0x80;
}

inline unsigned int _zebulon_write_block (unsigned long block, unsigned char data [512])
{
//	return trapR (trap_ide, ide_write_block, block, data);
	return 0x80;
}

}
#endif

