#include "../include/ide.h"
#include <stdio.h>

ide::register_access::register_access (MC68230& controller, unsigned char reg)
	: _controller (controller), _reg (reg)
{
	reset ();
}
		
unsigned char ide::register_access::read ()
{
	setup (eRead);
	pulse (eRead);
	unsigned char result = _controller.read_port_a ();
	reset ();
	return result;
}

void ide::register_access::write (unsigned char value)
{
	setup (eWrite);
	pulse (eWrite);
	_controller.write_port_a (value);
	reset ();
}


void ide::register_access::reset ()
{
	negate (0xFF);
	assert (eCS | eRead | eWrite);
	write_state ();
}

void ide::register_access::setup (unsigned char pin)
{
	negate (pin);
	write_state ();

	negate (eCS);
	assert (_reg);
	write_state ();
}

void ide::register_access::pulse (unsigned char pin)
{
	assert (pin);
	write_state ();

	negate (pin);
	write_state ();
}

void ide::register_access::negate (unsigned char bits)
{
	_state = _state & ~bits;
}

void ide::register_access::assert (unsigned char bits)
{
	_state = _state | bits;
}

void ide::register_access::write_state ()
{
	_controller.write_port_c (_state);
}


ide::ide (unsigned int base_address)
	: _controller (base_address)
{
	_controller.set_port_c_direction (0xFF);
}

void ide::test ()
{
	{
		unsigned char ds = read_register (eDeviceSelect);
		printf ("device select = %x\n\r",ds);
	}

	write_register (eDeviceSelect, 0xE0);

	{
		unsigned char ds = read_register (eDeviceSelect);
		printf ("device select = %x\n\r",ds);
	}
}

unsigned char ide::read_register (unsigned char reg)
{
	return access_register (reg).read ();
}

void ide::write_register (unsigned char reg, unsigned char value)
{
	return access_register (reg).write (value);
}


ide::register_access ide::access_register (unsigned char reg)
{
	return register_access (_controller, reg);
}
