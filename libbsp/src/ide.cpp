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
	
	set_address ();
	set_strobe (eRead, eNegate);

	unsigned char result = _controller.read_port_a ();

	set_strobe (eRead, eAssert);
	
	reset ();
	return result;
}

void ide::register_access::write (unsigned char value)
{
	_controller.set_port_a_direction (0xFF);

	set_address ();

	_controller.write_port_a (value);

	set_strobe (eWrite, eNegate);
	set_strobe (eWrite, eAssert);

	reset ();
}


void ide::register_access::reset ()
{
	negate_bits (0xFF);
	assert_bits (eCS | eRead | eWrite);
	write_state ();
}

void ide::register_access::set_address ()
{
	negate_bits (eCS);
	assert_bits (_reg);
	write_state ();
}

void ide::register_access::set_strobe (eStrobe strobe, eState state)
{
	if (state == eAssert)
		assert_bits (strobe);
	else
		negate_bits (strobe);
	write_state ();
}

void ide::register_access::negate_bits (unsigned char bits)
{
	_state = _state & ~bits;
}

void ide::register_access::assert_bits (unsigned char bits)
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

