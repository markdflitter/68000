#include "../include/bsp.h"
#include "../private_include/DUART.h"
#include "../private_include/IDE.h"

void* __memset (void* ptr, int value, size_t num)
{
	unsigned char* p = (unsigned char*) ptr;

	while (num--)
		*p++ = (unsigned char) value;

	return p;
}

void* __memcpy (void* destination, const void* source, size_t num)
{
	unsigned char* d = (unsigned char*) destination;
	const unsigned char* s = (unsigned char*) source;

	while (num--)
		*d++ = *s++;

	return destination;
}

void __init ()
{
}

DUART& the_duart ()
{
 	static DUART d;
	return d;
}

void __putch (char c)
{
	the_duart ().writeChar (DUART::channelA, c);
}

char __getch ()
{
	return the_duart ().readChar (DUART::channelA);
}

void __set_interrupt_vector (unsigned int vector)
{
	return the_duart ().setInterruptVector (vector);
}

void __enable_interrupts ()
{
	asm volatile ("andi #0xF8FF, %sr");
	the_duart ().enableInterrupts ();
}

void __disable_interrupts ()
{
	asm volatile ("ori #0x0700, %sr");
	the_duart ().disableInterrupts ();
}


IDE& the_ide ()
{
 	static IDE i;
	return i;
}

ide_result __ide_ident (DiskInfo& diskInfo)
{
	return (ide_result) the_ide ().ident (diskInfo);
}

ide_result __ide_write (unsigned long LBA, unsigned char data [512])
{
	return (ide_result) the_ide ().write (LBA, data);
}

ide_result __ide_read (unsigned long LBA, unsigned char data [512])
{
	return (ide_result) the_ide ().read (LBA, data);
}
