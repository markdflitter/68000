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

/*
extern unsigned char* __vector_table;

static unsigned int time = 0;

void tick () __attribute__ ((interrupt));
void tick ()
{
	printf ("tock\n\r");
	++time;
}
*/

int main ()
{
	printf ("%s\n\r", banner);	

/*
	VectorTable v (__vector_table);
	
	for (int i = 0; i < 256; i++)
		v.setVector (i, &tick);	

	__set_interrupt_vector (64);
	__enable_interrupts ();
	*/

	FAT f;
	Shell (f).run ();
	
	return 0;
}

