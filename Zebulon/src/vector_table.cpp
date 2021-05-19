#include "vector_table.h"
#include <stdio.h>


void unhandled_exception () __attribute__ ((interrupt));
void unhandled_exception ()
{
	printf ("* unhandled exception\n\r");
}


vector_table::vector_table (unsigned char* base_address)
	: _table ((table*) base_address)
{
	printf ("initialised vector table at 0x%x\n\r", base_address);

	for (int i = 0; i < 255; i++)
		set_vector (i, &unhandled_exception);
	printf ("set vectors\n\r");

	asm volatile ("move #0x2700, %sr");
	printf ("set sr\n\r");

	asm volatile ("move.l #0x200000, %a0");
// : : "m" (base_address));
	printf ("0 -> d0\n\r");

	asm volatile ("movec.l %a0, %vbr");
	printf ("set vbr\n\r");
}

vector_table::~vector_table ()
{
	asm volatile ("move.l #0, %d0");
	asm volatile ("movec %d0, %vbr");
}

void vector_table::set_vector (unsigned int vector, function_pointer function)
{
	_table->vectors [vector] = function;
}


