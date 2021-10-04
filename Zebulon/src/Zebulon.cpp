#include <bsp.h>
#include <stdio.h>
#include "VectorTable.h"
#include "Shell.h"

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
	volatile void* a0 = 0;
	asm volatile ("movel %%a0, %0\n\t" : "=m" (a0));

	double f = ((double) ticks) * tickIntervalInMs;

	*((unsigned int*) a0) = (unsigned int) f;
}

// putchar / getchar
void trap1 () __attribute__ ((interrupt));
void trap1 ()
{
	volatile char d0 = 0;
	volatile void* a0 = 0;
	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t" : "=m" (d0), "=m" (a0));

	switch (d0)
	{
		case 1:	__putch ((char) *((const int*) a0)); break;
		case 2: *((int*) a0) = (int) __getch (); break;
		default: break;
	}
}

/*
enum ide_result {OK = 0x0, AMNF = 0x1, TK0NF = 0x2, ABRT = 0x4, MCR = 0x8,
				IDNF = 0x10, MC = 0x20, UNC = 0x40, BBK = 0x80};
extern "C" ide_result __ide_ident (DiskInfo& result);
extern "C" ide_result __ide_write (unsigned long LBA, unsigned char data [512]);
extern "C" ide_result __ide_read (unsigned long LBA, unsigned char data [512]);
*/

// ident / read_block / write_block
void trap2 () __attribute__ ((interrupt));
void trap2 ()
{
	volatile char d0 = 0;
	volatile char d1 = 0;
	volatile void* a0 = 0;
	volatile void* a1 = 0;
	asm volatile ("moveb %%d0, %0\n\t" 
				  "moveb %%d1, %1\n\t" 
				  "movel %%a0, %1\n\t"
				  "movel %%a1, %1\n\t" : "=m" (d0), "=m" (d1), "=m" (a0), "=m" (a1));

	unsigned int result = IDE_OK;
	switch (d0)
	{
		case 1:	result = __ide_read (d1, (unsigned char*) a0); break;
		case 2: result = __ide_write (d1, (unsigned char*) a0); break;
		default: break;
	}

	*((unsigned int*) a1) = result;
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

