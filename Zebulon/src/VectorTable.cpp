#include "../private_include/VectorTable.h"
#include <stdio.h>
#include <bsp.h>


void unhandled () __attribute__ ((interrupt));
void unhandled ()
{
}

VectorTable::VectorTable (unsigned char* baseAddress)
	: m_table ((Table*) baseAddress)
{
	for (int i = 0; i < 255; i++)
		setVector (i, &unhandled);

 	asm ("movel %0, %%a0\n\t" 
		 "movecl %%a0, %%vbr\n\t" : : "a" (baseAddress) : "a0");
}

VectorTable::~VectorTable ()
{
	asm ("movel #0, %d0\n\t"
		 "movecl %d0, %vbr\n\t");
}

void VectorTable::setVector (unsigned int vector, fnPtr fn)
{
	m_table->m_vectors [vector] = fn;
}
