#include "../include/DUART.h"

DUART::UART::UART (unsigned int baseAddress) :
	m_registers ((Registers*) baseAddress)
{
}

bool DUART::UART::spaceToSend () const
{
	return (m_registers->sr & TX_READY) != 0;
}


void DUART::UART::writeChar (char c)
{
	while (!spaceToSend ())
		;
	
	m_registers->tx = c;
}

bool DUART::UART::charReady () const 
{
	return (m_registers->sr & RX_READY) != 0;
}


char DUART::UART::readChar () const
{
	while (!charReady ())
		;

	return m_registers->rx;
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


void DUART::setInterruptVector (unsigned int vector)
{
	m_registers->ivr = vector;
} 

const unsigned char TIMER_INTERRUPT = 8;

void DUART::enableInterrupts ()
{
	m_registers->imr = TIMER_INTERRUPT;
}

void DUART::disableInterrupts ()
{
	m_registers->imr = 0;
}
