#include <libgloss.h>
#include <libgloss_start.h>
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

