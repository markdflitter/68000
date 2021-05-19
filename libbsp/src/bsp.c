#include "../include/bsp.h"
#include "../include/DUART.h"
#include <stdio.h>


DUART& the_duart ()
{
 	static DUART d;

	return d;
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
