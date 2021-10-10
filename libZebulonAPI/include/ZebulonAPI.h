#ifndef ZEBULONAPI_H
#define ZEBULONAPI_H

#include "TrapHelper.h"
#include <stdio.h>

namespace Zebulon
{
	enum TRAP {trap_time = 0, trap_serial_IO = 1, trap_ide = 2, trap_filer = 3, trap_c_IO = 4, trap_file = 5};
	enum serial_IO_operations {serial_IO_write_char = 1, serial_IO_read_char = 2};
	enum ide_operations {ide_ident = 0, ide_write_block = 1, ide_read_block = 2};
	enum filer_operations {filer_format = 0, filer_diag = 1, filer_free_space = 2};
	enum c_IO_operations {c_IO_fopen = 0, c_IO_fclose = 1, c_IO_feof = 2, c_IO_fread = 3, c_IO_fwrite = 4};
	enum file_operations {file_stat = 0, file_delete = 1};

inline unsigned int _zebulon_time ()
{
	unsigned int result;
	trap (trap_time, &result);
	return result;
}

inline void _zebulon_putch (int c)
{
	// technically not volatile, but marking it so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it references the wrong stack location
	volatile int cc = c;
	trap (trap_serial_IO, serial_IO_write_char, &cc);
}

inline int _zebulon_getch ()
{
	int result;
	trap (trap_serial_IO, serial_IO_read_char, &result);
	return result;
}

enum ide_result {IDE_OK = 0x0, IDE_AMNF = 0x1, IDE_TK0NF = 0x2, IDE_ABRT = 0x4, IDE_MCR = 0x8,
				IDE_IDNF = 0x10, IDE_MC = 0x20, IDE_UNC = 0x40, IDE_BBK = 0x80};

struct DiskInfo {
	unsigned short general;
	bool hardSectored;
	bool softSectored;
	bool notMfmEncoded;
	bool headSwitchTime15uS;
	bool spindleMotorControlOptionImplemented;
	bool fixedDrive;
	bool removeableCartridgeDrive;
	bool transferRateLt5Mbs;
	bool transferRateLt10Mbs;
	bool transferRateGt10Mbs;
	bool rotationalSpeedTolerance;
	bool dataStrobeOffsetOptionAvailable;
	bool trackOffsetOptionAvailable;
	bool formatSpeedToleranceGapRequired;
	unsigned short numCylinders;
	unsigned short numHeads;
	unsigned short numBytesPerTrack;
	unsigned short numBytesPerSector;
	unsigned short numSectorsPerTrack;
	unsigned char serialNumber [21];
	unsigned short bufferType;
	unsigned short bufferSize;
	unsigned short numEccBytes;
	unsigned short firmwareRevision [9];
	unsigned char modelNumber[41];
	unsigned short maxRwSectorsPerInterrupt;
	bool doubleWordIO;
	unsigned short capabilities;
	bool LbaSupported;
	bool DmaSupported;
	unsigned short PioMode;
	unsigned short DmaMode;
	bool currentValid;
	unsigned short numCurrentCylinders;
	unsigned short numCurrentHeads;
	unsigned short numCurrentSectorsPerTrack;
	unsigned int currentCapacityInSectors;
	unsigned int totalNumOfUserSectors;
	unsigned short currentRwSectorsPerInterrupt;
	unsigned char singlewordDmaModesSupported;
	unsigned char singlewordDmaModesActive;
	unsigned char multiwordDmaModesSupported;
	unsigned char multiwordDmaModesActive;
};


inline unsigned int _zebulon_ide_ident (DiskInfo& info)
{
	unsigned int result;
	unsigned long ignore = 0;
	trap (trap_ide, ide_ident, ignore, &info, &result);
	return result;
}

inline unsigned int _zebulon_ide_read_block (unsigned long block, unsigned char data [512])
{
	unsigned int result;
	trap (trap_ide, ide_read_block, block, data, &result);
	return result;
}

inline unsigned int _zebulon_ide_write_block (unsigned long block, unsigned char data [512])
{
	unsigned int result;
	trap (trap_ide, ide_write_block, block, data, &result);
	return result;
}

inline int _zebulon_filer_format ()
{
	int result;
	trap (trap_filer, filer_format, &result);
	return result;
}

inline void _zebulon_filer_diag ()
{
	trap (trap_filer, filer_diag, 0);
}

struct FreeSpace
{
	unsigned long totalSpace;
	unsigned long freeSpace;
};

inline FreeSpace _zebulon_filer_free_space ()
{
	FreeSpace fs;
	trap (trap_filer, filer_free_space, &fs);
	return fs;
}

inline unsigned int _zebulon_fopen (const char* filename, const char* mode)
{
	int result;

	trap (trap_c_IO, c_IO_fopen, filename, mode, 0, &result);	

	return result == -1 ? 0 : result + 1;
}

inline void _zebulon_fclose (unsigned int fptr)
{
	trap (trap_c_IO, c_IO_fclose, (const volatile void*) (fptr - 1), 0, 0, 0);	
}

inline int _zebulon_feof (unsigned int fptr)
{
	int result;

	trap (trap_c_IO, c_IO_feof, (const volatile void*) (fptr -1), 0, 0, &result);	

	return result;
}


inline unsigned long _zebulon_fwrite (const void* data, long unsigned int data_size, long unsigned int number_data, unsigned int fptr)
{
	unsigned long result;
	unsigned long bytes = data_size * number_data;

	trap (trap_c_IO, c_IO_fwrite, ((const volatile void*) (fptr - 1)), data, (const volatile void*) bytes, &result);	

	return result;
}

inline unsigned long _zebulon_fread (const void* data, long unsigned int data_size, long unsigned int number_data, unsigned int fptr)
{
	unsigned long result;
	unsigned long bytes = data_size * number_data;

	trap (trap_c_IO, c_IO_fread, ((const volatile void*) (fptr - 1)), data, (const volatile void*) bytes, &result);	

	return result;
}

struct _zebulon_stats
{
	long unsigned int size;	
	long unsigned int sizeOnDisk;	
};

inline _zebulon_stats _zebulon_stat_file (const char* filename)
{
	_zebulon_stats result;

	trap (trap_file, file_stat, filename, 0, 0, &result);	

	return result;
}

inline unsigned int  _zebulon_delete_file (const char* filename)
{
	unsigned int result;

	trap (trap_file, file_delete, filename, 0, 0, &result);	

	return result;
}


}
#endif

