#include "../include/bsp.h"
#include "../include/duart.h"
#include <stdio.h>


duart& the_duart ()
{
 	static duart d;

	return d;
}


void __init ()
{
}

void __putch (char c)
{
	the_duart ().write_char (0, c);
}


char __getch ()
{
	return the_duart ().read_char (0);
}


void __set_interrupt_vector (unsigned int vector)
{
	return the_duart ().set_interrupt_vector (vector);
}

void __enable_interrupts ()
{
	return the_duart ().enable_interrupts ();
}

void __disable_interrupts ()
{
	return the_duart ().disable_interrupts ();
}



