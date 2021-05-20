#include "../include/bsp.h"
#include "../include/DUART.h"
#include "../include/IDE.h"

DUART& the_duart ()
{
 	static DUART d;
	return d;
}

IDE& the_ide ()
{
 	static IDE i;
	return i;
}


void __init ()
{
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
	return the_duart ().enableInterrupts ();
}

void __disable_interrupts ()
{
	return the_duart ().disableInterrupts ();
}

bool __ide_ident (DiskInfo& diskInfo)
{
	return the_ide ().ident (diskInfo);
}

bool __ide_write (unsigned long LBA, unsigned char data [512])
{
	return the_ide ().write (LBA, data);
}

bool __ide_read (unsigned long LBA, unsigned char data [512])
{
	return the_ide ().read (LBA, data);
}
