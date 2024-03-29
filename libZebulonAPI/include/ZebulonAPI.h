#ifndef ZEBULONAPI_H
#define ZEBULONAPI_H

#include "TrapHelper.h"

namespace Zebulon
{
	enum TRAP {trap_time = 0, trap_serial_IO = 1, trap_ide = 2, trap_filer = 3, trap_c_IO = 4, trap_file = 5, trap_file_search = 6, trap_boot = 7, trap_display = 8, trap_download = 9};
	enum serial_IO_operations {serial_IO_write_char = 1, serial_IO_read_char = 2, serial_IO_dld_char = 3};
	enum ide_operations {ide_ident = 0, ide_write_block = 1, ide_read_block = 2};
	enum filer_operations {filer_format = 0, filer_diag = 1, filer_free_space = 2, filer_heap_diag = 3};
	enum c_IO_operations {c_IO_fopen = 0, c_IO_fclose = 1, c_IO_feof = 2, c_IO_fread = 3, c_IO_fwrite = 4};
	enum file_operations {file_stat = 0, file_create = 1, file_delete = 2};
	enum file_search_operations {file_search_find_first = 0, file_search_find_next = 1, file_search_close = 2};
	enum boot_operations {boot_boot = 0, boot_index = 1};
	enum display_operations {display_set = 0};
	enum download_operations {download_download = 0, download_free = 1};

inline unsigned int _zebulon_time ()
{
	volatile unsigned int result;
	trap (trap_time, trap_params (0, &result));
	return result;
}

inline void _zebulon_putch (int c)
{
	// technically not volatile, but marking it so forces the compiler to make a local copy of the parameter which defeats a compiler bug where it references the wrong stack location
	volatile int cc = c;
	trap (trap_serial_IO, trap_params (serial_IO_write_char, &cc));
}

inline int _zebulon_getch ()
{
	volatile int result;
	trap (trap_serial_IO, trap_params (serial_IO_read_char, &result));
	return result;
}

inline char _zebulon_dldch ()
{
	volatile int result;
	trap (trap_serial_IO, trap_params (serial_IO_dld_char, &result));
	return result;
}



const unsigned int block_size = 512;

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
	volatile unsigned int result;
	unsigned long ignore = 0;
	trap (trap_ide, trap_params (ide_ident, &result, &info));
	return result;
}

inline unsigned int _zebulon_ide_read_block (unsigned long block, unsigned char data [block_size])
{
	volatile unsigned int result;
	trap (trap_ide, trap_params (ide_read_block, &result, (const void*) block, data));
	return result;
}

inline unsigned int _zebulon_ide_write_block (unsigned long block, unsigned char data [block_size])
{
	volatile unsigned int result;
	trap (trap_ide, trap_params (ide_write_block, &result, (const void*) block, data));
	return result;
}

inline int _zebulon_filer_format ()
{
	volatile int result;
	trap (trap_filer, trap_params (filer_format, &result));
	return result;
}

inline void _zebulon_filer_diag ()
{
	trap (trap_filer, trap_params (filer_diag));
}

inline void _zebulon_heap_diag (bool detail)
{
	trap (trap_filer, trap_params (filer_heap_diag, 0, (void*) detail));
}

struct _zebulon_free_space
{
	_zebulon_free_space ()
		: totalSpace (0), freeSpace (0)
	{
	}

	_zebulon_free_space (const _zebulon_free_space& other)
		: totalSpace (other.totalSpace), freeSpace (other.freeSpace)
	{
	}

	_zebulon_free_space (const volatile _zebulon_free_space& other)
		: totalSpace (other.totalSpace), freeSpace (other.freeSpace)
	{
	}

	unsigned long totalSpace;
	unsigned long freeSpace;
};

inline _zebulon_free_space _zebulon_filer_free_space ()
{
	volatile _zebulon_free_space fs;
	trap (trap_filer, trap_params (filer_free_space, &fs));
	return fs;
}

inline unsigned int _zebulon_fopen (const char* filename, const char* mode)
{
	volatile int result;

	trap (trap_c_IO, trap_params (c_IO_fopen, &result, filename, mode));	

	return result == -1 ? 0 : result + 1;
}

inline void _zebulon_fclose (unsigned int fptr)
{
	trap (trap_c_IO, trap_params (c_IO_fclose, 0, (const void*) (fptr - 1)));	
}

inline int _zebulon_feof (unsigned int fptr)
{
	volatile int result;

	trap (trap_c_IO, trap_params (c_IO_feof, &result, (const void*) (fptr -1)));	

	return result;
}


inline unsigned long _zebulon_fwrite (const void* data, long unsigned int data_size, long unsigned int number_data, unsigned int fptr)
{
	volatile unsigned long result;
	unsigned long bytes = data_size * number_data;

	trap (trap_c_IO, trap_params (c_IO_fwrite, &result, ((const void*) (fptr - 1)), data, (const void*) bytes));	

	return result;
}

inline unsigned long _zebulon_fread (const void* data, long unsigned int data_size, long unsigned int number_data, unsigned int fptr)
{
	volatile unsigned long result;
	unsigned long bytes = data_size * number_data;

	trap (trap_c_IO, trap_params (c_IO_fread, &result, ((const void*) (fptr - 1)), data, (const void*) bytes));	

	return result;
}

struct _zebulon_stats
{
	_zebulon_stats ()
		: size (0), sizeOnDisk (0)
	{
	}

	_zebulon_stats (const _zebulon_stats& other)
		: size (other.size), sizeOnDisk (other.sizeOnDisk)
	{
	}

	_zebulon_stats (const volatile _zebulon_stats& other)
		: size (other.size), sizeOnDisk (other.sizeOnDisk)
	{
	}

	long unsigned int size;	
	long unsigned int sizeOnDisk;	
};

inline int _zebulon_stat_file (const char* filename, _zebulon_stats* stats)
{
	volatile int result;

	trap (trap_file, trap_params (file_stat, &result, filename, stats));	

	return result;
}

inline bool _zebulon_create_file (const char* filename, unsigned long initialBlocks, bool contiguous)
{
	volatile unsigned int result;

	trap (trap_file, trap_params (file_create, &result, filename, (void*) initialBlocks, (void*) contiguous));	
	return result;
}

inline unsigned int  _zebulon_delete_file (const char* filename)
{
	volatile unsigned int result;

	trap (trap_file, trap_params (file_delete, &result, filename));	

	return result;
}

#define MAX_FILENAME_LENGTH 64
#define FILENAME_BUFFER_SIZE MAX_FILENAME_LENGTH+1

inline int _zebulon_find_first_file (char filename[FILENAME_BUFFER_SIZE])
{
	volatile int result;
	trap (trap_file_search, trap_params (file_search_find_first, &result, (void*) filename));
	return result;
}

inline bool _zebulon_find_next_file (int find_handle, char filename [FILENAME_BUFFER_SIZE])
{
	volatile bool result;
	trap (trap_file_search, trap_params (file_search_find_next, &result, (void*) filename, (void*) find_handle));
	return result;
}

inline void _zebulon_find_close (int find_handle)
{
	trap (trap_file_search, trap_params (file_search_close, 0, 0, (void*) find_handle));
}

inline bool _zebulon_boot_boot (unsigned int slot, const char* filename, unsigned int loadAddress, unsigned int startAddress, unsigned int length)
{
	volatile bool result;
	trap (trap_boot, trap_params (boot_boot, &result, (void*) slot, (void*) filename, (void*) loadAddress, (void*) startAddress, (void*) length));
	return result;
}

struct _zebulon_boot_table_entry
{
	bool empty;

 	char name [21];

	long unsigned int length;	
	unsigned int loadAddress;
	unsigned int startAddress;

	long unsigned int startBlock;
};

inline void _zebulon_index (_zebulon_boot_table_entry btes [9])
{
	trap (trap_boot, trap_params (boot_index, 0, (void*) btes));
}

inline void _zebulon_set_display (unsigned int num)
{
	trap (trap_display, trap_params (display_set, 0, (void*) num));
}

struct _zebulon_download_result
{
	_zebulon_download_result () : buffer (0), size (0) {}

	_zebulon_download_result (const _zebulon_download_result& other)
		: buffer(other.buffer), size (other.size)
	{
	}

	_zebulon_download_result (const volatile _zebulon_download_result& other)
		: buffer(other.buffer), size (other.size)
	{
	}

    char* buffer;
    unsigned int size;
};

inline _zebulon_download_result _zebulon_download_download ()
{
	volatile _zebulon_download_result result;
	trap (trap_download, trap_params (download_download, &result));
	return result;
}

inline void _zebulon_download_free (_zebulon_download_result result)
{
	trap (trap_download, trap_params (download_free, &result));
}

}
#endif

