#include <bsp.h>
#include <stdio.h>
#include "VectorTable.h"
#include "Shell.h"
#include <ZebulonAPI.h>
#include <TrapHelper.h>

const char* banner = 
"  ____\n\r"
" |    / ____________________________________\n\r"
"     /  ___  ___                ___         \n\r"
"    /  |    |   \\  |   |  |    /   \\  |\\   |\n\r"
" __/__ |__  | __/  |   |  |    | | |  | \\  |\n\r"
"  /    |    |   \\  |   |  |    | | |  |  \\ |\n\r" 
" /     |___ |___/  \\___/  |___ \\___/  |   \\|\n\r"
"/____| _____________________________________\n\r\n\r";

extern unsigned char* __vector_table;

static unsigned int ticks = 0;
static double tickIntervalInMs = 0.0;

void tick () __attribute__ ((interrupt));
void tick ()
{
	++ticks;
	__interrupt_handled ();
}


// time
void trap0 () __attribute__ ((interrupt));
void trap0 ()
{
	unsigned int* pResult = (unsigned int*) getA0 ();

	double f = ((double) ticks) * tickIntervalInMs;

	*pResult = (unsigned int) f;
}

// putchar / getchar
void trap1 () __attribute__ ((interrupt));
void trap1 ()
{
	char opcode = getOpcode ();
    int* pChar = (int*) getA0();	
	int c = (int) getP1();	
		
	switch (opcode)
	{
		case Zebulon::serialIO_write_char : __putch ((char) c); break;
		case Zebulon::serialIO_read_char  : *pChar = (int) __getch (); break;
		default: break;
	}
}

// ident / read_block / write_block
void trap2 () __attribute__ ((interrupt));
void trap2 ()
{
/*
	char opcode = getOpcode ();
   	volatile unsigned long block = getP1 ();
	volatile void* pResult = getA0 ();	
 	volatile void* pBuffer = getA1 ();
	
	unsigned int result = IDE_OK;
	switch (opcode)
	{
		case Zebulon::block_read_block  : result = __ide_read (block, (unsigned char*) pBuffer); break;
		case Zebulon::block_write_block : result = __ide_write (block, (unsigned char*) pBuffer); break;
		default: break;
	}

	*((unsigned int*) pResult) = result;
*/
}


int main ()
{
	__putstr (banner);	

	unsigned char* vectorBaseAddress = (unsigned char*) 0x200000;
	VectorTable v (vectorBaseAddress);	
	{
		char buf [200];
		sprintfn (buf, 200, "initialised vector table at 0x%x\n\r", vectorBaseAddress);
		__putstr (buf);
	}

	//start timer
	v.setVector (64, &tick);	

	tickIntervalInMs = __set_timer_divisor (0, 92);
	unsigned int d = tickIntervalInMs * 1000;

	__set_interrupt_vector (64);
	__enable_interrupts ();
   	{
		char buf [200];
		sprintfn (buf, 200, "enabled main timer: tick interval %duS\n\r", d);
		__putstr (buf);
	}

	// install traps
	v.setVector (32, &trap0);	
	v.setVector (33, &trap1);
	v.setVector (34, &trap2);

	// detailed diagnostics
	printf ("installed TRAPs\n\r");

	Shell ().run ();

	__disable_interrupts ();
	printf ("byebye\n\r");

	return 0;
}

