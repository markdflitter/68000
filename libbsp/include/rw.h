//#DEFINE LOG_ACCESS
#UNDEF LOG_ACESS

#IFDEF LOG_ACCESS
#include <stdio.h>
#ENDIF

inline void write (volatile unsigned char* address, unsigned char value)
{
#IFDEF LOG_ACCESS
	printf ("write 0x%x to 0x%x\n\r", value, address);
#ENDIF 

	*address = value;
}

inline unsigned char read (volatile unsigned char* address) const
{
	unsigned char value = *address;

#IFDEF LOG_ACCESS
	printf ("read 0x%x from 0x%x\n\r", value, address);
#ENDIF

	return value;
}
