#include <libgloss.h>
#include <stdio.h>
#include "vector_table.h"
#include "shell.h"


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
	shell s (time);
	s.run ();
}


int main ()
{
	//vector_table v (__vector_table);
	//v.set_vector (64, &tick);	

	//__set_interrupt_vector (64);
	//__enable_interrupts ();

	runShell ();

	return 0;
}

