#include <bsp.h>
#include <stdio.h>
#include "VectorTable.h"
#include "FAT.h"
#include "Shell.h"
#include <Zebulon.h>

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


file_handle openFile (const char* filename, const char* mode)
{
	file_handle result = file_not_found;

	FAT& fat = theFAT ();
	if (!fat.fileExists (filename))
	{
		//printf ("file doesn't exist\n\r");
		if (mode_is (mode, 'w'))
		{
			//printf ("create file\n\r");
	    	fat.create (filename);
		}
		else
		{
			//printf ("file not found\n\r");
			return result;
		}			
	}
	else
	{
		//printf ("file exists\n\r");
		if (mode_is (mode, 'w'))
		{
			//printf ("delete and create file\n\r");
			fat.rm (filename);
	    	fat.create (filename);
		}
	}

	result = fat.open (filename);

	if (mode_is (mode, 'r'))
		result |= 0x8000;
	
	return result;
}


unsigned long int readFile (file_handle fptr, unsigned char* data, long unsigned int numBytes)
{
	return theFAT ().read (fptr & ~0x8000, data, numBytes);
}

unsigned long int writeFile (file_handle fptr, const unsigned char* data, long unsigned int numBytes)
{
	return theFAT ().write (fptr & ~0x8000, data, numBytes);
}

void closeFile (file_handle fptr)
{
	theFAT ().close (fptr & ~0x8000);
}

int eof (file_handle fptr)
{
	return theFAT ().EOF (fptr & ~0x8000);
}

}

// fileIO
void trap2 () __attribute__ ((interrupt));
void trap2 ()
{
	volatile void* a0 = 0;
	volatile void* a1 = 0;
	volatile void* a2 = 0;
	volatile char d0 = 99;
	volatile unsigned long int d1 = 0;
	asm volatile ("moveb %%d0, %0\n\t" 
				  "movel %%a0, %1\n\t"
  				  "movel %%a1, %2\n\t"
  				  "movel %%a2, %3\n\t"
				  "movel %%d1, %4\n\t" : "=m" (d0), "=m" (a0), "=m" (a1), "=m" (a2), "=m" (d1));

	switch (d0)
	{
		case 1: *((file_handle *) a0) = openFile ((char*) a1, (char*) a2); break;
		case 2: *((long unsigned int*) a2) = writeFile (*((file_handle*) a0), (const unsigned char*) a1, (long unsigned int) d1); break ;
		case 3: *((long unsigned int*) a2) = readFile (*((file_handle*) a0), (unsigned char*) a1, (long unsigned int) d1); break ;
		case 4: closeFile (*((file_handle*) a0)); break;
		case 5: *((long unsigned int*) a2) = eof (*((file_handle*) a0)); break;
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


int main ()
{
	__putstr (banner);	

	VectorTable v ((unsigned char*) 0x200000);	
	v.setVector (32, &trap0);	
	v.setVector (33, &trap1);	
	v.setVector (34, &trap2);	
	v.setVector (35, &trap3);	
	v.setVector (64, &tick);	
	printf ("set up vectors 0x%x\n\r", __vector_table);

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

