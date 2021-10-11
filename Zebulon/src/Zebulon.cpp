#include <bsp.h>
#include <stdio.h>
#include "../private_include/VectorTable.h"
#include "../private_include/Shell.h"
#include <ZebulonAPI.h>
#include <TrapHelper.h>
#include <string.h>
#include "../private_include/Filer.h"

const char* banner = 
"  ____\n\r"
" |    / ____________________________________\n\r"
"     /  ___  ___                ___         \n\r"
"    /  |    |   \\  |   |  |    /   \\  |\\   |\n\r"
" __/__ |__  | __/  |   |  |    | | |  | \\  |\n\r"
"  /    |    |   \\  |   |  |    | | |  |  \\ |\n\r" 
" /     |___ |___/  \\___/  |___ \\___/  |   \\|\n\r"
"/____| _____________________________________\n\r\n\r";

extern char __vector_table;

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
	trap_params tp = untrap ();

	double f = ((double) ticks) * tickIntervalInMs;

	*((unsigned int*) tp.pResult) = (unsigned int) f;
}

// putchar / getchar
void trap1 () __attribute__ ((interrupt));
void trap1 ()
{
	trap_params tp = untrap ();

	switch (tp.opcode)
	{
		case Zebulon::serial_IO_write_char : __putch ((char) *((int*) tp.pResult)); break;
		case Zebulon::serial_IO_read_char  : *((int*) tp.pResult) = __getch (); break;
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
	trap_params tp = untrap ();

	unsigned int result = IDE_OK;
	switch (tp.opcode)
	{
		case Zebulon::ide_ident  :
		{
			::DiskInfo info;
			result = __ide_ident (info);
			convert_disk_info (&info, (Zebulon::DiskInfo*) tp.a1);
			break;
		}

		case Zebulon::ide_read_block  : result = __ide_read ((unsigned int) tp.a1, (unsigned char*) tp.a2); break;
		case Zebulon::ide_write_block : result = __ide_write ((unsigned int) tp.a1, (unsigned char*) tp.a2); break;
		default: break;
	}

	*((unsigned int*) tp.pResult) = result;
}

// filer
void trap3 () __attribute__ ((interrupt));
void trap3 ()
{
	trap_params tp = untrap ();
	
	switch (tp.opcode)
	{
		case Zebulon::filer_format:
		{
			::DiskInfo info;
			if (__ide_ident (info) == IDE_OK)
			{
				int numSectors = info.totalNumOfUserSectors;
				*((int*) tp.pResult) = theFiler ().format (numSectors);
			}
			else *((int*) tp.pResult) = -1;
			break;
		}
		case Zebulon::filer_diag: theFiler().diag (); break;
		case Zebulon::filer_free_space : *((Zebulon::_zebulon_free_space*) tp.pResult) = theFiler ().getFreeSpace (); break;
		default: break;
	}
}

// c file IO
void trap4 () __attribute__ ((interrupt));
void trap4 ()
{
	trap_params tp = untrap ();

	switch (tp.opcode)
	{
		case Zebulon::c_IO_fopen: * ((int*) tp.pResult) = theFiler ().fopen ((const char*) tp.a1, (const char*) tp.a2); break;
		case Zebulon::c_IO_fclose: theFiler ().fclose ((int) tp.a1); break;
		case Zebulon::c_IO_feof: * ((int*) tp.pResult) = theFiler ().feof ((int) tp.a1); break;
		case Zebulon::c_IO_fwrite: *((unsigned long*) tp.pResult) = theFiler ().fwrite ((int) tp.a1, (const unsigned char*) tp.a2, (unsigned long) tp.a3); break;
		case Zebulon::c_IO_fread: *((unsigned long*) tp.pResult) = theFiler ().fread ((int) tp.a1, (unsigned char*) tp.a2, (unsigned long) tp.a3); break;
		default: break;
	}
}

// file operations
void trap5 () __attribute__ ((interrupt));
void trap5 ()
{
	trap_params tp = untrap ();

	switch (tp.opcode)
	{
		case Zebulon::file_stat: *((int*) tp.pResult) = theFiler ().statFile ((const char*) tp.a1, (Zebulon::_zebulon_stats*) tp.a2); break;
		case Zebulon::file_delete: *((unsigned int*) tp.pResult) = theFiler ().deleteFile ((const char*) tp.a1); break;
		default: break;
	}
}

// file search operations
void trap6 () __attribute__ ((interrupt));
void trap6 ()
{
	trap_params tp = untrap ();

	switch (tp.opcode)
	{
		case Zebulon::file_search_find_first: *((int*) tp.pResult) = theFiler ().findFirstFile ((char*) tp.a1); break;
		case Zebulon::file_search_find_next: *((bool*) tp.pResult) = theFiler ().findNextFile ((int) tp.a2, (char*) tp.a1); break;
		case Zebulon::file_search_close: theFiler ().closeFind((int) tp.a2); break;

		default: break;
	}
}


int main ()
{
	__putstr (banner);	

	unsigned char* vectorBaseAddress = (unsigned char*) &__vector_table;
	{
		char buf [200];
		sprintfn (buf, 200, "initialising vector table at 0x%x\n\r", vectorBaseAddress);
		__putstr (buf);
	}
	VectorTable v (vectorBaseAddress);	

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
	v.setVector (37, &trap5);
	v.setVector (38, &trap6);


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
	int result = Shell ().run ();

	__disable_interrupts ();
	printf ("byebye\n\r");

	return result;
}
