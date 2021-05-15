#include "../include/ide.h"
#include <stdio.h>

const unsigned char IDENT_COMMAND = 0xEC;

const unsigned char DATA_REGISTER = 0x0;
const unsigned char ERROR_REGISTER = 0x1;
const unsigned char DRIVE_SELECT_REGISTER = 0x6;
const unsigned char STATUS_REGISTER = 0x7;
const unsigned char COMMAND_REGISTER = 0x7;

const unsigned char MASTER = 0xE0;

const unsigned char CHIP_SELECT_0 = 0x8; 
const unsigned char WRITE_STROBE = 0x10;
const unsigned char READ_STROBE = 0x20;

const unsigned char ERR = 0x1;
const unsigned char IDX = 0x2;
const unsigned char CORR = 0x4;
const unsigned char DRQ = 0x8;
const unsigned char DSC = 0x10;
const unsigned char DWF = 0x20;
const unsigned char DRDY = 0x40;
const unsigned char BUSY = 0x80;

const unsigned char AMNF = 0x1;
const unsigned char TK0NF = 0x2;
const unsigned char ABRT = 0x4;
const unsigned char MCR = 0x8;
const unsigned char IDNF = 0x10;
const unsigned char MC = 0x20;
const unsigned char UNC = 0x40;
const unsigned char BBK = 0x80;

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


bool ide::has_error ()
{
	return (read_status () & ERR) != 0;
}

void ide::print_error ()
{
	unsigned char error = read_register (ERROR_REGISTER);

	printf ("IDE ERROR:\n\r");
		
	if ((error & AMNF) != 0);
		printf ("  address mark not found\n\r");
	if ((error & TK0NF) != 0);
		printf ("  track 0 not found\n\r");
	if ((error & ABRT) != 0);
		printf ("  aborted command");
	if ((error & MCR) != 0);
		printf ("  media change requested\n\r");
	if ((error & IDNF) != 0);
		printf ("  ID not found\n\r");
	if ((error & MC) != 0);
		printf ("  media change\n\r");
	if ((error & UNC) != 0);
		printf ("  uncorrectable data error\n\r");
	if ((error & BBK) != 0);
		printf ("  bad block\n\r");
}


void ide::wait (unsigned char what)
{
	while ((read_status () & what) == 0x0)
		;
}

void ide::wait_not (unsigned char what)
{
	while ((read_status () & what) != 0x0)
		;
}

unsigned char ide::read_status ()
{
	return read_register (STATUS_REGISTER);
}

void ide::send_command (unsigned char command)
{
	wait_not (BUSY);
	write_register (COMMAND_REGISTER, command);
}

ide::register_access ide::access_register (unsigned char reg)
{
	return register_access (_controller, reg);
}

void ide::ident ()
{
	write_register (DRIVE_SELECT_REGISTER, MASTER);
	
	for (int j = 0; j < 10; j++)
		printf ("waiting...\n\r");

	wait (DRDY);

	for (unsigned char i = 0; i < 8; i++)
	{
		unsigned char ds = read_register (i);
		printf ("reg %d = %x\n\r",i, ds);
	}

	send_command (IDENT_COMMAND);
	wait_not (BUSY);

	if (has_error ())
	{
		print_error ();
		return ;
	}
	wait (DRQ);

	for (int i = 0; i < 256; i++)
	{
		unsigned char data = read_register (DATA_REGISTER);
		printf ("%c\n\r",data);
	}
}

