#include "../include/ide.h"
#include <stdio.h>

const unsigned char DRIVE_SELECT_REGISTER = 0x6;
const unsigned char STATUS_REGISTER = 0x7;
const unsigned char COMMAND_REGISTER = 0x7;

const unsigned char CHIP_SELECT_0 = 0x8; 
const unsigned char WRITE_STROBE = 0x10;
const unsigned char READ_STROBE = 0x20;

ide::register_access::register_access (MC68230& controller, unsigned char reg)
	: _controller (controller), _reg (reg)
{
	reset ();
}

unsigned char ide::register_access::read ()
{
	_controller.set_port_a_direction (MC68230::in);
	
	set_address ();
	set_strobe (READ_STROBE, negate);

	unsigned char result = _controller.read_port_a ();

	set_strobe (READ_STROBE, assert);
	
	reset ();
	return result;
}

void ide::register_access::write (unsigned char value)
{
	_controller.set_port_a_direction (MC68230::out);

	set_address ();

	_controller.write_port_a (value);

	set_strobe (WRITE_STROBE, negate);
	set_strobe (WRITE_STROBE, assert);

	reset ();
}


void ide::register_access::reset ()
{
	negate_bits (0xFF);
	assert_bits (CHIP_SELECT_0 | READ_STROBE | WRITE_STROBE);
	write_state ();
}

void ide::register_access::set_address ()
{
	negate_bits (CHIP_SELECT_0);
	assert_bits (_reg);
	write_state ();
}

void ide::register_access::set_strobe (unsigned char strobe, eState state)
{
	if (state == assert)
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
	_controller.set_port_c_direction (MC68230::out);
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

	for (unsigned char i = 0; i < 8; i++)
	{
		unsigned char ds = read_register (i);
		printf ("reg %d = %x\n\r",i, ds);
	}
}

