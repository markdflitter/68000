#include "../private_include/VectorTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <bsp.h>

void unhandled () __attribute__ ((interrupt));
void unhandled ()
{
}

// these need to be outside of the function because they affect the stack we are trying to read
unsigned int address;
char buffer [255];
		
void address_error () __attribute__ ((interrupt));
void address_error ()
{
	asm ("movel %%sp@(10), %0\n\t" : "=m" (address));
	
	address = ((address & 0xffff) << 16) | (address >> 16);	

	sprintf (buffer, "bad address 0x%x.  Aborting.\n\r", address);
	__putstr (buffer);	
	abort ();
}

void bus_error () __attribute__ ((interrupt));
void bus_error ()
{
	__putstr (">>> bus error.  Aborting.\n\r");
	abort ();
}

void illegal_instruction () __attribute__ ((interrupt));
void illegal_instruction ()
{
  	__putstr (">>> illegal instruction.  Aborting.\n\r");
	abort ();
}

void divide_by_zero () __attribute__ ((interrupt));
void divide_by_zero ()
{
  	__putstr (">>> divide by zero.  Aborting.\n\r");
	abort ();
}

void spurious_interrupt () __attribute__ ((interrupt));
void spurious_interrupt ()
{
 	__putstr (">>> spurious interrupt.\n\r");
}

VectorTable::VectorTable (unsigned char* baseAddress)
	: m_table ((Table*) baseAddress)
{
	for (int i = 0; i < 255; i++)
		setVector (i, &unhandled);
	
	setVector (2, &bus_error);
	setVector (3, &address_error);
	setVector (4, &illegal_instruction);
	setVector (5, &divide_by_zero);
	setVector (24, &spurious_interrupt);

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
