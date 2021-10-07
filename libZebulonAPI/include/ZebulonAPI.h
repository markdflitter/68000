#ifndef ZEBULONAPI_H
#define ZEBULONAPI_H

#include "TrapHelper.h"
#include <stdio.h>

namespace Zebulon
{
	enum TRAP {trap_time = 0, trap_serialIO = 1, trap_ide = 2};
	enum serialIO {serialIO_write_char = 1, serialIO_read_char = 2};
	enum ide_operations {ide_write_block = 1, ide_read_block = 2};

inline unsigned int _zebulon_time ()
{
	unsigned int result;
	trap (trap_time, &result);
	return result;
}

inline void _zebulon_putch (int c)
{
	// technically not volatile, but marking it so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it references the wrong stack location
	volatile int cc = c;
	//volatile void* a0 = &cc;
	trap (trap_serialIO, serialIO_write_char, &cc);
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
	unsigned int result;
	trap (trap_ide, ide_read_block, block, data, &result);
	return result;
}

inline unsigned int _zebulon_write_block (unsigned long block, unsigned char data [512])
{
	unsigned int result;
	trap (trap_ide, ide_write_block, block, data, &result);
	return result;
}

}
#endif

