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
	unsigned int* p = 0;
	asm volatile ("movel %%a0, %0\n\t" : "=m" (p));

	double f = ((double) ticks) * tickIntervalInMs;

	*p = (unsigned int) f;
}

// putchar / getchar
void trap1 () __attribute__ ((interrupt));
void trap1 ()
{
	int* p = 0;
	char operation = 0;
	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t" : "=m" (operation), "=m" (p));

	if (operation == 1)
		__putch ((char) *p);
	else if (operation == 2)
		*p = (int) __getch ();
}

namespace
{

bool mode_is (const char* mode, char what)
{
	char ch;
	while ((ch = *mode++) != '\0')
	{
		if (ch == what) return true;
	}
	
	return false;
}


int openFile (const char* filename, const char* mode)
{
	int result = -1;

	printf ("opening file %s in mode %s\n\r", filename, mode);
	
	FAT& fat = theFAT ();
	if (!fat.fileExists (filename))
	{
		printf ("file doesn't exist\n\r");
		if (mode_is (mode, 'w'))
		{
			printf ("create file\n\r");
	    	fat.create (filename);
		}
		else
		{
			printf ("file not found\n\r");
			return result;
		}			
	}
	else
	{
		printf ("file exists\n\r");
		if (mode_is (mode, 'w'))
		{
			printf ("delete and create file\n\r");
			fat.rm (filename);
	    	fat.create (filename);
		}
	}

	result = fat.open (filename);
	
	if (mode_is (mode, 'r'))
		result |= 0x8000;
	
	return result;
}

}


// fileIO

void trap2 () __attribute__ ((interrupt));
void trap2 ()
{
	char* filename = 0;
	char* mode = 0;
	int* result = 0;
	char operation = 0;
	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t"
  				  "movel %%a1, %2\n\t"
				  "movel %%a2, %3\n\t" : "=m" (operation), "=m" (filename), "=m" (mode), "=m" (result));

	switch (operation)
	{
		case 1: *result = openFile (filename, mode); break;
		default: break; 
	}
}

int main ()
{
	__putstr (banner);	

	VectorTable v (__vector_table);	
	v.setVector (32, &trap0);	
	v.setVector (33, &trap1);	
	v.setVector (34, &trap2);	
	v.setVector (64, &tick);	
	printf ("set up vectors\n\r");

	tickIntervalInMs = __set_timer_divisor (0, 92);
	unsigned int d = tickIntervalInMs * 1000;
	printf ("tick interval %duS\n\r", d);

	__set_interrupt_vector (64);
	__enable_interrupts ();
	printf ("enabled interrupts\n\r\n\r");

	Shell (theFAT ()).run ();

	__disable_interrupts ();
	printf ("byebye\n\r");

	return 0;
}

