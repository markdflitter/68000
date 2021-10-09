#include <bsp.h>
#include <stdio.h>
#include "VectorTable.h"
#include "Shell.h"
#include <ZebulonAPI.h>
#include <TrapHelper.h>
#include <string.h>
#include "Filer.h"

const char* banner = 
"  ____\n\r"
" |    / ____________________________________\n\r"
"     /  ___  ___                ___         \n\r"
"    /  |    |   \\  |   |  |    /   \\  |\\   |\n\r"
" __/__ |__  | __/  |   |  |    | | |  | \\  |\n\r"
"  /    |    |   \\  |   |  |    | | |  |  \\ |\n\r" 
" /     |___ |___/  \\___/  |___ \\___/  |   \\|\n\r"
"/____| _____________________________________\n\r\n\r";

extern unsigned char* __vector_table;

static unsigned int ticks = 0;
static double tickIntervalInMs = 0.0;


Zebulon::Filer& theFiler ()
{
	static Zebulon::Filer f;
	return f;
}


void tick () __attribute__ ((interrupt));
void tick ()
{
	++ticks;
	__interrupt_handled ();
}


// time
void trap0 () __attribute__ ((interrupt));
void trap0 ()
{
	unsigned int* pResult = (unsigned int*) untrap ();

	double f = ((double) ticks) * tickIntervalInMs;

	*pResult = (unsigned int) f;
}

// putchar / getchar
void trap1 () __attribute__ ((interrupt));
void trap1 ()
{
	unsigned char opcode;
	int* p = (int*) untrap (opcode);
		
	switch (opcode)
	{
		case Zebulon::serial_IO_write_char : __putch ((char) *p); break;
		case Zebulon::serial_IO_read_char  : *p = (int) __getch (); break;
		default: break;
	}
}

void convert_disk_info (::DiskInfo* source, Zebulon::DiskInfo* target)
{
	target->general = source->general;

	target->hardSectored = source->hardSectored;
	target->softSectored = source->softSectored;
	target->notMfmEncoded = source->notMfmEncoded;
	target->headSwitchTime15uS = source->headSwitchTime15uS;
	source->spindleMotorControlOptionImplemented = source->spindleMotorControlOptionImplemented;
	target->fixedDrive = source->fixedDrive;
	target->removeableCartridgeDrive = source->removeableCartridgeDrive;
	target->transferRateLt5Mbs = source->transferRateLt5Mbs;
	target->transferRateLt10Mbs = source->transferRateLt10Mbs;
	target->transferRateGt10Mbs = source->transferRateGt10Mbs;
	target->rotationalSpeedTolerance = source->rotationalSpeedTolerance;
	target->dataStrobeOffsetOptionAvailable = source->dataStrobeOffsetOptionAvailable;
	target->trackOffsetOptionAvailable = source->trackOffsetOptionAvailable;
	target->formatSpeedToleranceGapRequired = source->formatSpeedToleranceGapRequired;

	target->numCylinders = source->numCylinders;
	target->numHeads = source->numHeads;
	target->numBytesPerTrack = source->numBytesPerTrack;
	target->numBytesPerSector = source->numBytesPerSector;
	target->numSectorsPerTrack = source->numSectorsPerTrack;
	memcpy (target->serialNumber, source->serialNumber, 21);
	target->bufferSize = source->bufferSize;
	target->numEccBytes = source->numEccBytes;
 	
	memcpy (target->firmwareRevision, source->firmwareRevision, 9);
	memcpy (target->modelNumber, source->modelNumber, 41);

	target->maxRwSectorsPerInterrupt = source->maxRwSectorsPerInterrupt;
	target->doubleWordIO = source->doubleWordIO;
	target->capabilities = source->capabilities;
	target->DmaSupported = source->DmaSupported;
	target->LbaSupported = source->LbaSupported;
	target->PioMode = source->PioMode;
	target->DmaMode = source->DmaMode;

	target->currentValid =source->currentValid;
	target->numCurrentCylinders = source->numCurrentCylinders;
	target->numCurrentHeads = source->numCurrentHeads;

	target->numCurrentSectorsPerTrack = source->numCurrentSectorsPerTrack;
	target->currentCapacityInSectors = source->currentCapacityInSectors;
	
	target->currentRwSectorsPerInterrupt = source->currentRwSectorsPerInterrupt;
	target->totalNumOfUserSectors = source->totalNumOfUserSectors;
			
	target->singlewordDmaModesSupported = source->singlewordDmaModesSupported;
	target->singlewordDmaModesActive = source->singlewordDmaModesActive;
	target->multiwordDmaModesActive = source->multiwordDmaModesActive;
}


// ident / read_block / write_block
void trap2 () __attribute__ ((interrupt));
void trap2 ()
{
	unsigned char opcode;
	unsigned long block;
	void* data;
	unsigned int* pResult = (unsigned int*) untrap (opcode, block, data);

	unsigned int result = IDE_OK;
	switch (opcode)
	{
		case Zebulon::ide_ident  :
		{
			::DiskInfo info;
			result = __ide_ident (info);
			convert_disk_info (&info, (Zebulon::DiskInfo*) data);
			break;
		}

		case Zebulon::ide_read_block  : result = __ide_read (block, (unsigned char*) data); break;
		case Zebulon::ide_write_block : result = __ide_write (block, (unsigned char*) data); break;
		default: break;
	}

	*pResult = result;
}

// filer
void trap3 () __attribute__ ((interrupt));
void trap3 ()
{
	unsigned char opcode;
	int* pResult = (int*) untrap (opcode);
	
	switch (opcode)
	{
		case Zebulon::filer_format:
		{
			::DiskInfo info;
			if (__ide_ident (info) == IDE_OK)
			{
				int numSectors = info.totalNumOfUserSectors;
				*pResult = theFiler ().format (numSectors);
			}
			else *pResult = -1;
			break;
		}
		case Zebulon::filer_diag: theFiler().diag (); break;
		case Zebulon::filer_free_space : *((Zebulon::FreeSpace*) pResult) = theFiler ().getFreeSpace (); break;
		default: break;
	}
}

// c file IO
void trap4 () __attribute__ ((interrupt));
void trap4 ()
{
	unsigned char opcode;
	const volatile void* a1;
	const volatile void* a2;
	const volatile void* a3;
	void* pResult = (int*) untrap (opcode, a1, a2, a3);

	switch (opcode)
	{
		case Zebulon::c_IO_fopen: * ((int*) pResult) = theFiler ().fopen ((const char*) a1, (const char*) a2); break;
		case Zebulon::c_IO_fclose: theFiler ().fclose ((int) a1); break;
		case Zebulon::c_IO_fwrite: *((unsigned long*) pResult) = theFiler ().fwrite ((int) a1, (const unsigned char*) a2, (unsigned long) a3); break;
			default: break;
	}
}

int main ()
{
	__putstr (banner);	

	unsigned char* vectorBaseAddress = (unsigned char*) 0x200000;
	VectorTable v (vectorBaseAddress);	
	{
		char buf [200];
		sprintfn (buf, 200, "initialised vector table at 0x%x\n\r", vectorBaseAddress);
		__putstr (buf);
	}

	//start timer
	v.setVector (64, &tick);	

	tickIntervalInMs = __set_timer_divisor (0, 92);
	unsigned int d = tickIntervalInMs * 1000;

	__set_interrupt_vector (64);
	__enable_interrupts ();
   	{
		char buf [200];
		sprintfn (buf, 200, "enabled main timer: tick interval %duS\n\r", d);
		__putstr (buf);
	}

	// install traps
	v.setVector (32, &trap0);	
	v.setVector (33, &trap1);
	v.setVector (34, &trap2);
	v.setVector (35, &trap3);
	v.setVector (36, &trap4);



	// detailed diagnostics
	printf ("installed TRAPs\n\r");

	::DiskInfo info;
	if (__ide_ident (info) == IDE_OK)
	{
		double capacity = ((double) info.totalNumOfUserSectors) * 512 / 1000000000;
		printf ("identified %s (%d Gb)\n\r", info.modelNumber, (unsigned long) capacity);
		printf (" serial number\t\t: %s\n\r",info.serialNumber);
		printf (" firmware revision\t: %s\n\r",info.firmwareRevision);
	}
	
	theFiler ().load ();
	
	printf ("\n\r");
	Shell ().run ();

	__disable_interrupts ();
	printf ("byebye\n\r");

	return 0;
}
