#include "../include/duart.h"
#include <stdio.h>

duart::uart::uart (unsigned int base_address) :
	_rm ((uart_register_map*) base_address)
{
}

bool duart::uart::space_to_send () const
{
	return (_rm->sr & TX_READY) != 0;
}


void duart::uart::write_char (char c)
{
	printf ("%x",&(_rm->tx));
	while (!space_to_send ())
		;
	
	_rm->tx = c;
}

bool duart::uart::char_ready () const 
{
	return (_rm->sr & RX_READY) != 0;
}


char duart::uart::read_char () const
{
	while (!char_ready ())
		;

	return _rm->rx;
}


duart::duart (unsigned int base_address) : 
	_rm ((duart_register_map*) base_address),
	_a (base_address),
	_b (base_address + 16)
{
	printf ("%s", "intialise duart\n\r");
	_uart [0] = &_a;
	_uart [1] = &_b;
}

void duart::write_char (int channel, char c)
{
	_uart [channel]->write_char (c);
}

char duart::read_char (int channel) const
{
	return _uart[channel]->read_char ();
}

