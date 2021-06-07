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
	void* a0 = 0;
	asm volatile ("movel %%a0, %0\n\t" : "=m" (a0));

	double f = ((double) ticks) * tickIntervalInMs;

	*((unsigned int*) a0) = (unsigned int) f;
}

// putchar / getchar
void trap1 () __attribute__ ((interrupt));
void trap1 ()
{
	char d0 = 0;
	void* a0 = 0;
	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t" : "=m" (d0), "=m" (a0));

	switch (d0)
	{
		case 1:	__putch ((char) *((const int*) a0)); break;
		case 2: *((int*) a0) = (int) __getch (); break;
		default: break;
	}
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


unsigned long int readFile (int fptr, unsigned char* data, long unsigned int numBytes)
{
	return theFAT ().read (fptr, data, numBytes);
}

unsigned long int writeFile (int fptr, const unsigned char* data, long unsigned int numBytes)
{
	return theFAT ().write (fptr, data, numBytes);
}

void closeFile (int fptr)
{
	theFAT ().close (fptr & ~0x8000);
}

}


// fileIO

void trap2 () __attribute__ ((interrupt));
void trap2 ()
{
	void* a0 = 0;
	void* a1 = 0;
	void* a2 = 0;
	char d0 = 0;
	char d1 = 0;
	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t"
  				  "movel %%a1, %2\n\t"
  				  "movel %%a2, %2\n\t"
				  "movel %%d1, %4\n\t" : "=m" (d0), "=m" (a0), "=m" (a1), "=m" (a2), "=m" (d1));

	switch (d0)
	{
		case 1: *((int *) a0) = openFile ((char*) a1, (char*) a2); break;
		case 2: *((long unsigned int*) a2) = writeFile (*((int*) a0), (const unsigned char*) a1, (long unsigned int) d1); break ;
		case 3: *((long unsigned int*) a2) = readFile (*((int*) a0), (unsigned char*) a1, (long unsigned int) d1); break ;
		case 4: closeFile (*((int*) a0));
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

