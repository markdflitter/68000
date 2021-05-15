#include "../include/ide.h"
#include <stdio.h>
#include <string.h>

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

unsigned char ide::register_access::read8 ()
{
	_controller.set_port_b_direction (MC68230::in);
	
	set_address ();
	set_strobe (READ_STROBE, negate);

	unsigned char result = _controller.read_port_b ();

	set_strobe (READ_STROBE, assert);
	
	reset ();
	return result;
}

void ide::register_access::write8 (unsigned char value)
{
	_controller.set_port_b_direction (MC68230::out);

	set_address ();

	_controller.write_port_b (value);

	set_strobe (WRITE_STROBE, negate);
	set_strobe (WRITE_STROBE, assert);

	reset ();
}

unsigned short ide::register_access::read16 ()
{
	_controller.set_port_b_direction (MC68230::in);
	_controller.set_port_a_direction (MC68230::in);
			
	set_address ();
	set_strobe (READ_STROBE, negate);

	unsigned char lsb = _controller.read_port_b ();
	unsigned char msb = _controller.read_port_a ();

	set_strobe (READ_STROBE, assert);

	unsigned short result = msb;
	result = result << 8;
	result = result | lsb;

	reset ();
	return result;
}

void ide::register_access::write16 (unsigned short value)
{
	_controller.set_port_b_direction (MC68230::out);
	_controller.set_port_a_direction (MC68230::out);
	
	set_address ();

	_controller.write_port_b (value & 0xFF);
	_controller.write_port_a (value >> 8);

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
	_controller.set_port_b_control (0x40);
	_controller.set_port_c_direction (MC68230::out);
}

unsigned char ide::read_register (unsigned char reg)
{
	return access_register (reg).read8 ();
}

void ide::write_register (unsigned char reg, unsigned char value)
{
	return access_register (reg).write8 (value);
}

unsigned short ide::read_data ()
{
	return access_register (DATA_REGISTER).read16 ();
}

void ide::write_data (unsigned short value)
{
	return access_register (DATA_REGISTER).write16 (value);
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

bool ide::ident (disk_info& result)
{
	write_register (DRIVE_SELECT_REGISTER, MASTER);
	wait (DRDY);

	send_command (IDENT_COMMAND);
	wait_not (BUSY);

	if (has_error ())
	{
		print_error ();
		return false;
	}
	wait (DRQ);

	unsigned short response [256];
	for (int i = 0; i < 256; i++)
	{
		unsigned short data = read_data ();
		response [i] = data;
	}

	result.general = response [0];
	result.num_cylinders = response [1];
	result.num_heads = response [3];
	result.num_bytes_per_track = response [4];
	result.num_bytes_per_sector = response [5];
	result.num_sectors_per_track = response [6];
	memcpy (&(result.serial_number), &(response [10]), 2 * 10);

	result.buffer_type = response [20];
	result.buffer_size = response [21];
	result.num_ECC_bytes = response [22];
	memcpy (&(result.firmware_revision), &(response [23]), 2 * 4);
	memcpy (&(result.model_number), &(response [27]), 2 * 20);

	result.double_word_io = response [48];
	result.capabilities = response [49];
	result.PIO_mode = response [51];
	result.DMA_mode = response [52];

	result.num_current_cylinders = response [54];
	result.num_current_heads = response [55];
	result.num_current_sectors_per_track = response [56];
	memcpy (&(result.current_capacity_in_sectors), &(response [57]), 2 * 2);
	
	memcpy (&(result.total_num_of_user_sectors), &(response [60]), 2 * 2);
	result.singleword_DMA = response [62];
	result.multiword_DMA = response [63];

	return true;	
}

