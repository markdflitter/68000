#include <bsp.h>
#include <stdio.h>
#include "VectorTable.h"
#include "FAT.h"
#include "Shell.h"
#include <Zebulon.h>

using namespace Zebulon;

const char* banner = 
"  ____\n\r"
" |    / ____________________________________\n\r"
"     /  ___  ___                ___         \n\r"
"    /  |    |   \\  |   |  |    /   \\  |\\   |\n\r"
" __/__ |__  | __/  |   |  |    | | |  | \\  |\n\r"
"  /    |    |   \\  |   |  |    | | |  |  \\ |\n\r" 
" /     |___ |___/  \\___/  |___ \\___/  |   \\|\n\r"
"/____| _____________________________________\n\r\n\r";


FAT& theFAT ()
{
	static FAT fat;
	return fat;
}

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

// fileIO
void trap2 () __attribute__ ((interrupt));
void trap2 ()
{
	volatile void* a0 = 0;
	volatile void* a1 = 0;
	volatile void* a2 = 0;
	volatile char d0 = 0;
	volatile unsigned long int d1 = 0;
	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t"
  				  "movel %%a1, %2\n\t"
  				  "movel %%a2, %3\n\t"
				  "movel %%d1, %4\n\t" : "=m" (d0), "=m" (a0), "=m" (a1), "=m" (a2), "=m" (d1));

	switch (d0)
	{
		case 1: *((file_handle *) a0) = theFAT ().open ((char*) a1, (char*) a2); break;
		case 2: *((long unsigned int*) a2) = theFAT ().write (*((file_handle*) a0), (const unsigned char*) a1, (long unsigned int) d1); break ;
		case 3: *((long unsigned int*) a2) = theFAT ().read (*((file_handle*) a0), (unsigned char*) a1, (long unsigned int) d1); break ;
		case 4: theFAT ().close (*((file_handle*) a0)); break;
		case 5: *((long unsigned int*) a2) = theFAT ().EOF (*((file_handle*) a0)); break;
		default: break; 
	}
}

// stat
void trap3 () __attribute__ ((interrupt));
void trap3 ()
{
	volatile void* a0 = 0;
	volatile void* a1 = 0;
	volatile void* a2 = 0;
	asm volatile ("movel %%a0, %0\n\t"
  				  "movel %%a1, %1\n\t"
  				  "movel %%a2, %2\n\t" : "=m" (a0), "=m" (a1), "=m" (a2));

	*((bool*) a2) = theFAT ().stat ((const char*) a0, ((struct Zebulon::_zebulon_stat*) a1));
}

void trap4 () __attribute__ ((interrupt));
void trap4 ()
{
	volatile char d0 = 0;
	volatile void* a0 = 0;
	volatile void* a1 = 0;
	volatile void* a2 = 0;

	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t"
  				  "movel %%a1, %2\n\t"
  				  "movel %%a2, %3\n\t" : "=m" (d0), "=m" (a0), "=m" (a1), "=m" (a2));

	switch (d0)
	{
		case 1: *((int*) a2) = theFAT ().findFirstFile (((struct Zebulon::_zebulon_stat*) a0)); break;
		case 2: *((bool*) a2) = theFAT ().findNextFile (*((int*) a1), ((struct Zebulon::_zebulon_stat*) a0)); break;
		case 3: theFAT ().closeFind (*((int*) a1)); break;
		default: break;
	}
}

//file operations: delete, copy etc
void trap5 () __attribute__ ((interrupt));
void trap5 ()
{
	volatile void* a0 = 0;

	asm volatile ("movel %%a0, %0\n\t" : "=m" (a0));

	theFAT ().rm (((char*) a0));
}

//boot operations
void trap6 () __attribute__ ((interrupt));
void trap6 ()
{
	volatile char d0 = 0;
	volatile void* a0 = 0;
	volatile void* a1 = 0;

	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t"
  				  "movel %%a1, %2\n\t" : "=m" (d0), "=m" (a0), "=m" (a1));
	switch (d0)
	{
		case 1: theFAT ().boot (((const char*) a1), *((unsigned int*) a0)); break;
		case 2: theFAT ().unboot (*((unsigned int*) a0)); break;
		case 3: theFAT ().index (((_zebulon_boot_table_entry*) a0)); break;
		default: break;
	}
}

//format
void trap7 () __attribute__ ((interrupt));
void trap7 ()
{
	volatile void* a0 = 0;

	asm volatile ("movel %%a0, %0\n\t" : "=m" (a0));

	theFAT ().format (*((unsigned int*) a0));
}

//dumpBlock
void trap8 () __attribute__ ((interrupt));
void trap8 ()
{
	volatile void* a0 = 0;

	asm volatile ("movel %%a0, %0\n\t" : "=m" (a0));

	theFAT ().dumpBlock (*((unsigned int*) a0));
}


int main ()
{
	__putstr (banner);	

	VectorTable v ((unsigned char*) 0x200000);	
	v.setVector (32, &trap0);	
	v.setVector (33, &trap1);	
	v.setVector (34, &trap2);	
	v.setVector (35, &trap3);	
	v.setVector (36, &trap4);	
	v.setVector (37, &trap5);	
	v.setVector (38, &trap6);	
	v.setVector (39, &trap7);	
	v.setVector (40, &trap8);	
	v.setVector (64, &tick);	
	printf ("set up vectors 0x%x\n\r", __vector_table);

	tickIntervalInMs = __set_timer_divisor (0, 92);
	unsigned int d = tickIntervalInMs * 1000;
	printf ("tick interval %duS\n\r", d);

	__set_interrupt_vector (64);
	__enable_interrupts ();
	printf ("enabled interrupts\n\r\n\r");

	Shell::run ();

	__disable_interrupts ();
	printf ("byebye\n\r");

	return 0;
}

