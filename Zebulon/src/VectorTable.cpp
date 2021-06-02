#include "VectorTable.h"
#include <stdio.h>

void unhandled () __attribute__ ((interrupt));
void unhandled ()
{
}

VectorTable::VectorTable (unsigned char* baseAddress)
	: m_table ((Table*) baseAddress)
{
	printf ("initialised vector table at 0x%x\n\r", baseAddress);

	for (int i = 0; i < 255; i++)
		setVector (i, &unhandled);

 	asm volatile ("move.l %0, %%a0\n\t" 
				  "movec.l %%a0, %%vbr\n\t" : : "a" (baseAddress) : "a0");
}

VectorTable::~VectorTable ()
{
	asm volatile ("move.l #0, %d0\n\t"
				  "movec %d0, %vbr\n\t");
}

void VectorTable::setVector (unsigned int vector, fnPtr fn)
{
	m_table->m_vectors [vector] = fn;
}


