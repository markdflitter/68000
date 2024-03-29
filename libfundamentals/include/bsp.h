#ifndef BSP_H
#define BSP_H

#include "size_t.h"

extern "C" void __init ();

extern "C" void* __memset (void* ptr, int value, size_t num);
extern "C" void* __memcpy (void* destination, const void* source, size_t num);

extern "C" void __putch (char c);
extern "C" void __putstr (const char* s);
extern "C" char __getch ();
extern "C" char __dldch ();

extern "C" void __set_interrupt_vector (unsigned int vector);
extern "C" void __enable_interrupts ();
extern "C" void __disable_interrupts ();
extern "C" void __interrupt_handled ();
extern "C" double __set_timer_divisor (unsigned char msb, unsigned char lsb);

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

const size_t ide_block_size = 512;

typedef unsigned int (*get_time_fn_ptr) ();

enum ide_result {IDE_OK = 0x0, IDE_AMNF = 0x1, IDE_TK0NF = 0x2, IDE_ABRT = 0x4, IDE_MCR = 0x8,
				IDE_IDNF = 0x10, IDE_MC = 0x20, IDE_UNC = 0x40, IDE_BBK = 0x80, IDE_TIMEOUT = 0x100};
extern "C" ide_result __ide_ident (DiskInfo& result, get_time_fn_ptr get_time, unsigned int timeout_time);
extern "C" ide_result __ide_write (unsigned long LBA, unsigned char data [ide_block_size], get_time_fn_ptr get_time = 0, unsigned int timeout_time = 0);
extern "C" ide_result __ide_read (unsigned long LBA, unsigned char data [ide_block_size], get_time_fn_ptr get_time = 0, unsigned int timeout_time = 0);

extern "C" void __set_display (unsigned int num);

#endif
