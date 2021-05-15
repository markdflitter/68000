#include "../include/ide.h"
#include <stdio.h>

ide::register_access::register_access (MC68230& controller, unsigned char reg)
	: _controller (controller), _reg (reg)
{
	reset ();
}

unsigned char ide::register_access::read ()
{
	_controller.set_port_a_direction (0x0);
	
	setup (eRead);
	pulse (eRead);
	unsigned char result = _controller.read_port_a ();
	assert (eCS | eRead | eWrite);
	write_state ();
	reset ();
	return result;
}

void ide::register_access::write (unsigned char value)
{
	_controller.set_port_a_direction (0xFF);

	setup (eWrite);
	_controller.write_port_a (value);
	pulse (eWrite);
	assert (eCS | eRead | eWrite);
	write_state ();
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
	negate (eCS);
	assert (_reg);
	write_state ();
}

void ide::register_access::pulse (unsigned char pin)
{
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
	_controller.set_general_control (0x0);
	_controller.set_port_a_control (0x40);
	_controller.set_port_c_direction (0xFF);
}

void ide::test ()
{
	write_register (0x6, 0xE0);

	for (int j = 0; j < 10; j++)
		printf ("waiting...\n\r");

	write_register (0x4, 0xAB);

	printf ("0x%x\n\r",read_register (0x4));


	//write_register (0x7, 0xE0);


	for (unsigned char i = 0; i < 8; i++)
	{
		unsigned char ds = read_register (i);
		printf ("reg %d = %x\n\r",i, ds);
	}

//	write_register (eBlockAddress0_7, 0x87654321);

//	{
//		unsigned char ds = read_register (eBlockAddress0_7);
//		printf ("device select = %x\n\r",ds);
//	}
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
