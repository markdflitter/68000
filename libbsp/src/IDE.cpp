#include "../private_include/IDE.h"
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

IDE::RegisterAccess::RegisterAccess (MC68230& controller, unsigned char reg)
	: m_controller (controller), m_register (reg)
{
	reset ();
}

unsigned char IDE::RegisterAccess::read8 ()
{
	m_controller.setPortBDirection (MC68230::in);
	
	setAddress ();
	setStrobe (READ_STROBE, negate);

	unsigned char result = m_controller.readPortB ();

	setStrobe (READ_STROBE, assert);
	
	reset ();
	return result;
}

void IDE::RegisterAccess::write8 (unsigned char value)
{
	m_controller.setPortBDirection (MC68230::out);

	setAddress ();

	m_controller.writePortB (value);

	setStrobe (WRITE_STROBE, negate);
	setStrobe (WRITE_STROBE, assert);

	reset ();
}

unsigned short IDE::RegisterAccess::read16 ()
{
	m_controller.setPortBDirection (MC68230::in);
	m_controller.setPortADirection (MC68230::in);			
	setAddress ();
	setStrobe (READ_STROBE, negate);

	unsigned char lsb = m_controller.readPortB ();
	unsigned char msb = m_controller.readPortA ();

	setStrobe (READ_STROBE, assert);

	unsigned short result = msb;
	result = result << 8;
	result = result | lsb;

	reset ();
	return result;
}

void IDE::RegisterAccess::write16 (unsigned short value)
{
	m_controller.setPortBDirection (MC68230::out);
	m_controller.setPortADirection (MC68230::out);
	
	setAddress ();

	m_controller.writePortB (value & 0xFF);
	m_controller.writePortA (value >> 8);

	setStrobe (WRITE_STROBE, negate);
	setStrobe (WRITE_STROBE, assert);

	reset ();
}

void IDE::RegisterAccess::reset ()
{
	negateBits (0xFF);
	assertBits (CHIP_SELECT_0 | READ_STROBE | WRITE_STROBE);
	writeState ();
}

void IDE::RegisterAccess::setAddress ()
{
	negateBits (CHIP_SELECT_0);
	assertBits (m_register);
	writeState ();
}

void IDE::RegisterAccess::setStrobe (unsigned char strobe, eState state)
{
	if (state == assert)
		assertBits (strobe);
	else
		negateBits (strobe);
	writeState ();
}

void IDE::RegisterAccess::negateBits (unsigned char bits)
{
	m_state = m_state & ~bits;
}

void IDE::RegisterAccess::assertBits (unsigned char bits)
{
	m_state = m_state | bits;
}

void IDE::RegisterAccess::writeState ()
{
	m_controller.writePortC (m_state);
}


IDE::IDE (unsigned int baseAddress)
	: m_controller (baseAddress)
{
	m_controller.setGeneralControl (0x0);
	m_controller.setPortAControl (0x40);
	m_controller.setPortBControl (0x40);
	m_controller.setPortCDirection (MC68230::out);
}

unsigned char IDE::readRegister (unsigned char reg)
{
	return accessRegister (reg).read8 ();
}

void IDE::writeRegister (unsigned char reg, unsigned char value)
{
	return accessRegister (reg).write8 (value);
}

unsigned short IDE::readData ()
{
	return accessRegister (DATA_REGISTER).read16 ();
}

void IDE::writeData (unsigned short value)
{
	return accessRegister (DATA_REGISTER).write16 (value);
}

bool IDE::hasError ()
{
	return (readStatus () & ERR) != 0;
}

IDE::Result IDE::error ()
{
	unsigned char error = readRegister (ERROR_REGISTER);
	if ((error & AMNF) != 0);
		 return AMNF;
	if ((error & TK0NF) != 0);
		return TK0NF;
	if ((error & ABRT) != 0);
		return ABRT;
	if ((error & MCR) != 0);
		return MCR;
	if ((error & IDNF) != 0);
		return IDNF;
	if ((error & MC) != 0);
		return MC;
	if ((error & UNC) != 0);
		return UNC;
	if ((error & BBK) != 0);
		return BBK;
}

void IDE::wait (unsigned char what)
{
	while ((readStatus () & what) == 0x0)
		;
}

void IDE::waitNot (unsigned char what)
{
	while ((readStatus () & what) != 0x0)
		;
}

unsigned char IDE::readStatus ()
{
	return readRegister (STATUS_REGISTER);
}

void IDE::sendCommand (unsigned char command)
{
	waitNot (BUSY);
	writeRegister (COMMAND_REGISTER, command);
}

IDE::RegisterAccess IDE::accessRegister (unsigned char reg)
{
	return RegisterAccess (m_controller, reg);
}

IDE::Result IDE::ident (DiskInfo& result)
{
	writeRegister (DRIVE_SELECT_REGISTER, MASTER);
	wait (DRDY);

	sendCommand (IDENT_COMMAND);
	waitNot (BUSY);

	if (hasError ())
	{
		return error ();
	}
	wait (DRQ);

	unsigned short response [256];
	for (int i = 0; i < 256; i++)
	{
		unsigned short data = readData ();
		response [i] = data;
	}

	result.general = response [0];
	result.hardSectored = result.general & 0x2;
	result.softSectored = result.general & 0x4;
	result.notMfmEncoded = result.general & 0x8;
	result.headSwitchTime15uS = result.general & 0x10;
	result.spindleMotorControlOptionImplemented = result.general & 0x20;
	result.fixedDrive = result.general & 0x40;
	result.removeableCartridgeDrive = result.general & 0x80;
	result.transferRateLt5Mbs = result.general & 0x100;
	result.transferRateLt10Mbs = result.general & 0x200;
	result.transferRateGt10Mbs = result.general & 0x400;
	result.rotationalSpeedTolerance = result.general & 0x800;
	result.dataStrobeOffsetOptionAvailable = result.general & 0x1000;
	result.trackOffsetOptionAvailable = result.general & 0x2000;
	result.formatSpeedToleranceGapRequired = result.general & 0x4000;

	result.numCylinders = response [1];
	result.numHeads = response [3];
	result.numBytesPerTrack = response [4];
	result.numBytesPerSector = response [5];
	result.numSectorsPerTrack = response [6];
	__memcpy (&(result.serialNumber), &(response [10]), 2 * 10);

	result.bufferType = response [20];
	result.bufferSize = response [21];
	result.numEccBytes = response [22];
	__memcpy (&(result.firmwareRevision), &(response [23]), 2 * 4);
	__memcpy (&(result.modelNumber), &(response [27]), 2 * 20);

	result.maxRwSectorsPerInterrupt = response [47] & 0xFF;
	
	result.doubleWordIO = response [48] & 0x1;
	result.capabilities = response [49];
	result.DmaSupported = result.capabilities & 0x100;
	result.LbaSupported = result.capabilities & 0x200;
	result.PioMode = response [51] >> 8;
	result.DmaMode = response [52] >> 8;

	result.currentValid = response [53] & 0x1;
	result.numCurrentCylinders = response [54];
	result.numCurrentHeads = response [55];
	result.numCurrentSectorsPerTrack = response [56];
	__memcpy (&(result.currentCapacityInSectors), &(response [57]), 2 * 2);
	result.currentRwSectorsPerInterrupt = response [59] & 0xFF;
	__memcpy (&(result.totalNumOfUserSectors), &(response [60]), 2 * 2);
			
	result.singlewordDmaModesSupported = response [62] & 0xFF;
	result.singlewordDmaModesActive = response [62] >> 8;
	result.multiwordDmaModesSupported = response [63] & 0xFF;
	result.multiwordDmaModesActive = response [63] >> 8;
	
	return OK;
}

void IDE::setLBA (unsigned long LBA)
{
	writeRegister (SECTOR_COUNT_REGISTER, 1);
	writeRegister (LBA_0_REGISTER, LBA & 0xFF);
	writeRegister (LBA_1_REGISTER, (LBA & 0xFF00) >> 8);
	writeRegister (LBA_2_REGISTER, (LBA & 0xFF0000) >> 16);
	writeRegister (LBA_3_REGISTER, MASTER | ((LBA & 0x0F000000) >> 24));
}


IDE::Result  IDE::write (unsigned long LBA, unsigned char data [512])
{
	writeRegister (DRIVE_SELECT_REGISTER, MASTER);
	wait (DRDY);

	setLBA (LBA);

	sendCommand (WRITE_SECTORS_WITH_RETRY);

	waitNot (BUSY);

	if (hasError ())
	{
		return error ();
	}

	wait (DRQ);

	for (int i = 0; i < 512; i = i + 2)
	{
		unsigned short w;
		__memcpy (&w, &(data [i]), 2);

		writeData (w);
	}

	return OK;
}

IDE::Result IDE::read (unsigned long LBA, unsigned char data [512])
{
	writeRegister (DRIVE_SELECT_REGISTER, MASTER);
	wait (DRDY);

	setLBA (LBA);

	sendCommand (READ_SECTORS_WITH_RETRY);

	waitNot (BUSY);

	if (hasError ())
	{
		return error ();
	}

	wait (DRQ);

	for (int i = 0; i < 512; i = i + 2)
	{
		unsigned short w = readData ();
		__memcpy (&(data [i]),&w, 2);
	}

	return OK;
}
