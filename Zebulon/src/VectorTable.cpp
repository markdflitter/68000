#include "VectorTable.h"
#include <stdio.h>


void unhandled () __attribute__ ((interrupt));
void unhandled ()
{
	printf ("* unhandled exception\n\r");
}


VectorTable::VectorTable (unsigned char* baseAddress)
	: m_table ((Table*) baseAddress)
{
	printf ("initialised vector table at 0x%x\n\r", baseAddress);

	for (int i = 0; i < 255; i++)
		setVector (i, &unhandled);
	printf ("set vectors\n\r");

	asm volatile ("move #0x2700, %sr");
 	asm volatile ("move.l #0x200004, %a0");
	asm volatile ("movec.l %a0, %vbr");
}

VectorTable::~VectorTable ()
{
	asm volatile ("move.l #0, %d0");
	asm volatile ("movec %d0, %vbr");
}

void VectorTable::setVector (unsigned int vector, fnPtr fn)
{
	m_table->m_vectors [vector] = fn;
}


