#include <bsp.h>
#include <stdio.h>
#include "VectorTable.h"
#include "FAT.h"
#include "Shell.h"

const char* banner = 
"  ____\n\r"
" |    / ____________________________________\n\r"
"     /  ___  ___                ___         \n\r"
"    /  |    |   \\  |   |  |    /   \\  |\\   |\n\r"
" __/__ |__  | __/  |   |  |    | | |  | \\  |\n\r"
"  /    |    |   \\  |   |  |    | | |  |  \\ |\n\r" 
" /     |___ |___/  \\___/  |___ \\___/  |   \\|\n\r"
"/____| _____________________________________\n\r";


extern unsigned char* __vector_table;

static unsigned int ticks = 0;
static double tickIntervalInMs = 0.0;

void tick () __attribute__ ((interrupt));
void tick ()
{
	++ticks;
	__interrupt_handled ();
}

void trap0 () __attribute__ ((interrupt));
void trap0 ()
{
	unsigned int* result;
	asm ("move.l %%a0, %0\n\t" : "=m" (result));

	double f = ((double) ticks) * tickIntervalInMs;
	unsigned int timeInMs (f);
	*result = timeInMs;
}

int main ()
{
	printf ("%s\n\r", banner);	

	VectorTable v (__vector_table);	
	v.setVector (32, &trap0);	
	v.setVector (64, &tick);	

	tickIntervalInMs = __set_timer_divisor (0, 23);
	unsigned int d = tickIntervalInMs * 1000;
	printf ("tick interval %duS\n\r", d);

	__set_interrupt_vector (64);
	__enable_interrupts ();

	FAT f;
	Shell (f).run ();

	__disable_interrupts ();
	
	return 0;
}

