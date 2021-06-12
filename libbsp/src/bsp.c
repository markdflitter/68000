#include "../include/bsp.h"
#include "../private_include/DUART.h"
#include "../private_include/IDE.h"

DUART& the_duart ()
{
 	static DUART d;
	return d;
}

void __putch (char c)
{
	the_duart ().writeChar (DUART::channelA, c);
}

void __putstr (const char* c)
{
	while (*c != '\0')
		__putch (*c++);
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
	asm ("andi #0xF8FF, %sr");
	the_duart ().enableInterrupts ();
}

void __disable_interrupts ()
{
	asm ("ori #0x0700, %sr");
	the_duart ().disableInterrupts ();
}

void __interrupt_handled ()
{
	the_duart ().clearInterrupts ();
}

double __set_timer_divisor (unsigned char msb, unsigned char lsb)
{
	the_duart ().setTimerDivisor (msb, lsb);
	unsigned int divisor = msb;
	divisor = (divisor << 8) | lsb;
	double ticksPerSecond = 3685000.0l / 16 / divisor / 2;
	double tickIntervalInS = 1.0 / ticksPerSecond;
	double tickIntervalInMs = 1000 * tickIntervalInS;
			
	return tickIntervalInMs;
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
