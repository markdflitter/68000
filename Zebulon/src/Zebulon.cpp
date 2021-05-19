#include <bsp.h>
#include <stdio.h>
#include "VectorTable.h"
#include "Shell.h"


extern unsigned char* __vector_table;

static unsigned int time = 0;

void tick () __attribute__ ((interrupt));
void tick ()
{
	printf ("tock\n\r");
	++time;
}


void runShell ()
{
	Shell s (time);
	s.run ();
}


int main ()
{
	//VectorTable v (__vector_table);
	//v.setVector (64, &tick);	

	//__set_interrupt_vector (64);
	//__enable_interrupts ();

	runShell ();

	return 0;
}

