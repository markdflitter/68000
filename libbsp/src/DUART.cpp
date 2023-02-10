#include "../private_include/DUART.h"
#include "../private_include/rw.h"
#include <stdio.h>
#include <bsp.h>

DUART::UART::UART (unsigned int baseAddress) :
	m_registers ((Registers*) baseAddress)
{
}

bool DUART::UART::spaceToSend () const
{
	return (read (&m_registers->sr) & TX_READY) != 0;
}


void DUART::UART::writeChar (char c)
{
	while (!spaceToSend ())
		;
	
	write (&m_registers->tx, c);
}

inline bool DUART::UART::charReady () const 
{
	return (read (&m_registers->sr) & RX_READY) != 0;
}


inline char DUART::UART::readChar () const
{
	while (!charReady ())
		;

	return read (&m_registers->rx);
}

char* DUART::UART::download (unsigned int* fileSize) const 
{
	int dld = readChar ();
    *fileSize = 0;

	while (dld != '|')
		dld = readChar ();
    dld = readChar ();
    dld = readChar ();
 	
	while (dld != '|')
	{
		if (dld >= '0' && dld <= '9')
		{
        	*fileSize *= 10;
			*fileSize += dld - '0';
		}
		dld = readChar ();
	}

	printf ("expecting %d bytes\n\r",*fileSize);

    dld = readChar ();
    dld = readChar ();
 
	char* buffer = new char [*fileSize];
	register char* p = buffer;
    register unsigned int bytesLeft = *fileSize;

	int counter = 0;
	while (bytesLeft)
	{
		while (!(m_registers->sr & RX_READY))
			;

		while ((m_registers->sr & RX_READY) && bytesLeft)
		{
			*p++ = m_registers->rx;
			--bytesLeft;

			if (counter++ == 512)
			{
				__putch ('.');
				counter = 0;
			}
			__set_display (*(p-1) & 0xF);
		}
	}

	printf ("\n\r");
	return buffer;
}


DUART::DUART (unsigned int baseAddress) : 
	m_registers ((Registers*) baseAddress),
	m_a (baseAddress),
	m_b (baseAddress + 16)
{
	m_uart [0] = &m_a;
	m_uart [1] = &m_b;
}

void DUART::writeChar (eChannel channel, char c)
{
	m_uart [channel]->writeChar (c);
}

char DUART::readChar (eChannel channel) const
{
	return m_uart [channel]->readChar ();
}


char* DUART::download (eChannel channel, unsigned int* fileSize) const
{
	return m_uart [channel]->download (fileSize);
}


void DUART::setInterruptVector (unsigned int vector)
{
	write (&m_registers->ivr, vector);
} 

const unsigned char TIMER_INTERRUPT = 8;

void DUART::enableInterrupts ()
{
	write (&m_registers->imr, TIMER_INTERRUPT);
}

void DUART::disableInterrupts ()
{
	write (&m_registers->imr, 0);
}

void DUART::clearInterrupts ()
{
	read (&m_registers->stop_counter);
}

void DUART::setTimerDivisor (unsigned char msb, unsigned char lsb)
{
	write (&m_registers->ctur, msb);
	write (&m_registers->ctlr, lsb);
}
