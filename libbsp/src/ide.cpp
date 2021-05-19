#include "../include/ide.h"
#include <stdio.h>
#include <string.h>

const unsigned char READ_SECTORS_WITH_RETRY = 0x20;
const unsigned char WRITE_SECTORS_WITH_RETRY = 0x30;
const unsigned char IDENT_COMMAND = 0xEC;

const unsigned char DATA_REGISTER = 0x0;
const unsigned char ERROR_REGISTER = 0x1;
const unsigned char SECTOR_COUNT_REGISTER = 0x2;
const unsigned char LBA_0_REGISTER = 0x3;
const unsigned char LBA_1_REGISTER = 0x4;
const unsigned char LBA_2_REGISTER = 0x5;
const unsigned char LBA_3_REGISTER = 0x6;
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
	_controller.setPortBDirection (MC68230::in);
	
	set_address ();
	set_strobe (READ_STROBE, negate);

	unsigned char result = _controller.readPortB ();

	set_strobe (READ_STROBE, assert);
	
	reset ();
	return result;
}

void ide::register_access::write8 (unsigned char value)
{
	_controller.setPortBDirection (MC68230::out);

	set_address ();

	_controller.writePortB (value);

	set_strobe (WRITE_STROBE, negate);
	set_strobe (WRITE_STROBE, assert);

	reset ();
}

unsigned short ide::register_access::read16 ()
{
	_controller.setPortBDirection (MC68230::in);
	_controller.setPortADirection (MC68230::in);			
	set_address ();
	set_strobe (READ_STROBE, negate);

	unsigned char lsb = _controller.readPortB ();
	unsigned char msb = _controller.readPortA ();

	set_strobe (READ_STROBE, assert);

	unsigned short result = msb;
	result = result << 8;
	result = result | lsb;

	reset ();
	return result;
}

void ide::register_access::write16 (unsigned short value)
{
	_controller.setPortBDirection (MC68230::out);
	_controller.setPortADirection (MC68230::out);
	
	set_address ();

	_controller.writePortB (value & 0xFF);
	_controller.writePortA (value >> 8);

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
	_controller.writePortC (_state);
}


ide::ide (unsigned int base_address)
	: _controller (base_address)
{
	_controller.setGeneralControl (0x0);
	_controller.setPortAControl (0x40);
	_controller.setPortBControl (0x40);
	_controller.setPortCDirection (MC68230::out);
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
	result.hard_sectored = result.general & 0x2;
	result.soft_sectored = result.general & 0x4;
	result.not_MFM_encoded = result.general & 0x8;
	result.head_switch_time_15uS = result.general & 0x10;
	result.spindle_motor_control_option_implemented = result.general & 0x20;
	result.fixed_drive = result.general & 0x40;
	result.removeable_cartridge_drive = result.general & 0x80;
	result.transfer_rate_lt_5Mbs = result.general & 0x100;
	result.transfer_rate_lt_10Mbs = result.general & 0x200;
	result.transfer_rate_gt_10Mbs = result.general & 0x400;
	result.rotational_speed_tolerance = result.general & 0x800;
	result.data_strobe_offset_option_available = result.general & 0x1000;
	result.track_offset_option_available = result.general & 0x2000;
	result.format_speed_tolerance_gap_required = result.general & 0x4000;

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

	result.max_rw_sectors_per_interrupt = response [47] & 0xFF;
	
	result.double_word_io = response [48] & 0x1;
	result.capabilities = response [49];
	result.DMA_supported = result.capabilities & 0x100;
	result.LBA_supported = result.capabilities & 0x200;
	result.PIO_mode = response [51] >> 8;
	result.DMA_mode = response [52] >> 8;

	result.current_valid = response [53] & 0x1;
	result.num_current_cylinders = response [54];
	result.num_current_heads = response [55];
	result.num_current_sectors_per_track = response [56];
	memcpy (&(result.current_capacity_in_sectors), &(response [57]), 2 * 2);
	result.current_rw_sectors_per_interrupt = response [59] & 0xFF;
	memcpy (&(result.total_num_of_user_sectors), &(response [60]), 2 * 2);
			
	result.singleword_DMA_modes_supported = response [62] & 0xFF;
	result.singleword_DMA_modes_active = response [62] >> 8;
	result.multiword_DMA_modes_supported = response [63] & 0xFF;
	result.multiword_DMA_modes_active = response [63] >> 8;
	
	return true;	
}

void ide::set_lba (unsigned long lba)
{
	write_register (SECTOR_COUNT_REGISTER, 1);
	write_register (LBA_0_REGISTER, lba & 0xFF);
	write_register (LBA_1_REGISTER, (lba & 0xFF00) >> 8);
	write_register (LBA_2_REGISTER, (lba & 0xFF0000) >> 16);
	write_register (LBA_3_REGISTER, MASTER | ((lba & 0x0F000000) >> 24));
}


bool ide::write (unsigned long lba, unsigned char data [512])
{
	write_register (DRIVE_SELECT_REGISTER, MASTER);
	wait (DRDY);

	set_lba (lba);

	send_command (WRITE_SECTORS_WITH_RETRY);

	wait_not (BUSY);

	if (has_error ())
	{
		print_error ();
		return false;
	}

	wait (DRQ);

	for (int i = 0; i < 512; i = i + 2)
	{
		unsigned short w;
		memcpy (&w, &(data [i]), 2);

		write_data (w);
	}

	return true;
}

bool ide::read (unsigned long lba, unsigned char data [512])
{
	write_register (DRIVE_SELECT_REGISTER, MASTER);
	wait (DRDY);

	set_lba (lba);

	send_command (READ_SECTORS_WITH_RETRY);

	wait_not (BUSY);

	if (has_error ())
	{
		print_error ();
		return false;
	}

	wait (DRQ);

	for (int i = 0; i < 512; i = i + 2)
	{
		unsigned short w = read_data ();
		memcpy (&(data [i]),&w, 2);
	}

	return true;
}

