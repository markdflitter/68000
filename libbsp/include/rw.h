#ifndef RW_H
#define RW_H

//#define LOG_ACCESS
#undef LOG_ACESS

#if LOG_ACCESS
#include <stdio.h>
#endif

inline void write (volatile unsigned char* address, unsigned char value)
{
#ifdef LOG_ACCESS
	printf ("write 0x%x to 0x%x\n\r", value, address);
#endif 

	*address = value;
}

inline unsigned char read (volatile unsigned char* address) const
{
	unsigned char value = *address;

#ifdef LOG_ACCESS
	printf ("read 0x%x from 0x%x\n\r", value, address);
#endif

	return value;
}

#endif
