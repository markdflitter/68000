#include <bsp.h>
#include <stdio.h>
#include "../private_include/VectorTable.h"
#include "../private_include/Shell.h"
#include <ZebulonAPI.h>
#include <TrapHelper.h>
#include <string.h>
#include "../private_include/Filer.h"
#include <stdlib.h>

using namespace std;
using namespace Zebulon;

const char* banner = 
"  ____\n\r"
" |    / ____________________________________\n\r"
"     /  ___  ___                ___         \n\r"
"    /  |    |   \\  |   |  |    /   \\  |\\   |\n\r"
" __/__ |__  | __/  |   |  |    | | |  | \\  |\n\r"
"  /    |    |   \\  |   |  |    | | |  |  \\ |\n\r" 
" /     |___ |___/  \\___/  |___ \\___/  |   \\|\n\r"
"/____| _____________________________________\n\r\n\r";

extern char __vector_table[];

static volatile unsigned int ticks = 0;
static double tickIntervalInMs = 0.0;
static unsigned int diskTimeoutInMS = 50;

Filer& theFiler ()
{
	static Filer f;
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
		case serial_IO_write_char : __putch ((char) *((int*) tp.pResult)); break;
		case serial_IO_read_char  : *((int*) tp.pResult) = __getch (); break;
		case serial_IO_dld_char   : *((int*) tp.pResult) = __dldch (); break;
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

	unsigned int result = Zebulon::IDE_OK;
	switch (tp.opcode)
	{
		case ide_ident  :
		{
			::DiskInfo info;
			result = __ide_ident (info, _zebulon_time, _zebulon_time () + diskTimeoutInMS);
			convert_disk_info (&info, (Zebulon::DiskInfo*) tp.a1);
			break;
		}

		case ide_read_block  : result = __ide_read ((unsigned int) tp.a1, (unsigned char*) tp.a2); break;
		case ide_write_block : result = __ide_write ((unsigned int) tp.a1, (unsigned char*) tp.a2); break;
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
		case filer_format:
		{
			::DiskInfo info;
			if (__ide_ident (info, _zebulon_time, _zebulon_time () + diskTimeoutInMS) == ::IDE_OK)
			{
				int numSectors = info.totalNumOfUserSectors;
				*((int*) tp.pResult) = theFiler ().guard ()->format (numSectors);
			}
			else 
				*((int*) tp.pResult) = -1;
			break;
		}
		case filer_diag: theFiler().diag (); break;
		case filer_free_space : *((_zebulon_free_space*) tp.pResult) = theFiler ().getFreeSpace (); break;
		case filer_heap_diag: heap_diag ((bool) tp.a1); break;
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
		case c_IO_fopen: * ((int*) tp.pResult) = theFiler ().fopen ((const char*) tp.a1, (const char*) tp.a2); break;
		case c_IO_fclose: theFiler ().fclose ((int) tp.a1); break;
		case c_IO_feof: * ((int*) tp.pResult) = theFiler ().feof ((int) tp.a1); break;
		case c_IO_fwrite: *((unsigned long*) tp.pResult) = theFiler ().fwrite ((int) tp.a1, (const unsigned char*) tp.a2, (unsigned long) tp.a3); break;
		case c_IO_fread: *((unsigned long*) tp.pResult) = theFiler ().fread ((int) tp.a1, (unsigned char*) tp.a2, (unsigned long) tp.a3); break;
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
		case file_stat: *((int*) tp.pResult) = theFiler ().statFile ((const char*) tp.a1, (_zebulon_stats*) tp.a2); break;
		case file_create: *((bool*) tp.pResult) = theFiler ().guard ()->createFile ((const char*) tp.a1, (unsigned long) tp.a2, (bool) tp.a3); break;
			case file_delete: *((unsigned int*) tp.pResult) = theFiler ().guard ()->deleteFile ((const char*) tp.a1); break;
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
		case file_search_find_first: *((int*) tp.pResult) = theFiler ().findFirstFile ((char*) tp.a1); break;
		case file_search_find_next: *((bool*) tp.pResult) = theFiler ().findNextFile ((int) tp.a2, (char*) tp.a1); break;
		case file_search_close: theFiler ().closeFind((int) tp.a2); break;

		default: break;
	}
}

// boot operations
void trap7 () __attribute__ ((interrupt));
void trap7 ()
{
	trap_params tp = untrap ();

	switch (tp.opcode)
	{
		case boot_boot: *((bool*) tp.pResult) = theFiler ().guard ()->boot ((unsigned int) tp.a1, string ((char*) tp.a2), (unsigned int) tp.a3, (unsigned int) tp.a4, (unsigned int) tp.a5); break;
		case boot_index: theFiler ().index ((_zebulon_boot_table_entry*) tp.a1); break;
		default: break;
	}
}

// display operations
void trap8 () __attribute__ ((interrupt));
void trap8 ()
{
	trap_params tp = untrap ();

	switch (tp.opcode)
	{
		case display_set: __set_display ((unsigned int) tp.a1); break;
		default: break;
	}
}

void do_download_download (_zebulon_download_result* result)
{
	if (!result) return;

	result->size = 0;
	result->buffer = 0;

	int size = 0;

	int downloaded_char = __dldch ();
	
	while (downloaded_char != '|')
		downloaded_char = __dldch ();
    downloaded_char = __dldch ();
    downloaded_char = __dldch ();
 	
	while (downloaded_char != '|')
	{
		if (downloaded_char >= '0' && downloaded_char <= '9')
		{
        	size *= 10;
			size += downloaded_char - '0';
		}
		downloaded_char = __dldch ();
	}

	printf ("expecting %d bytes\n\r",size);

    downloaded_char = __dldch ();
    downloaded_char = __dldch ();
 
	char* buffer = new char [size];
	char* p = buffer;
    unsigned int bytesLeft = size;

	int counter = 0;
	while (bytesLeft)
	{
			*p++ = __dldch ();
			--bytesLeft;

			if (counter++ == 512)
			{
				__putch ('.');
				counter = 0;
				__set_display (*(p-1) & 0xF);
			}
	}

	result->size = size;
	result->buffer = buffer;

	printf ("\n\r");
}

void do_download_free (_zebulon_download_result* result)
{
  if (result && result->buffer)
  {
	delete[] result->buffer;
	result->buffer = 0;
	result->size = 0;
  }
}


// download operations
void trap9 () __attribute__ ((interrupt));
void trap9 ()
{
	trap_params tp = untrap ();

	switch (tp.opcode)
	{
		case download_download: do_download_download ((_zebulon_download_result*) tp.pResult); break;
		case download_free: do_download_free ((_zebulon_download_result*) tp.pResult); break;
		default: break;
	}
}


void test_heap ()
{
    heap_diag (true);

	int * p1;
	{
		printf ("1 calling malloc()\n\r");
		p1 = new int;
		printf ("1 called malloc()\n\r");
	}
    heap_diag (true);

	int* p2;
	{
		printf ("2 calling malloc()\n\r");
		p2 = new int [2];
		printf ("2 called malloc()\n\r");
	}
    heap_diag (true);

	{
		printf ("100 calling free()\n\r");
		delete p2;
		printf ("100 called free()\n\r");
	}
    heap_diag (true);

	{
		printf ("200 calling free()\n\r");
		delete p1;
		printf ("200 called free()\n\r");
	}
    heap_diag (true);

	{
		printf ("3 calling malloc()\n\r");
		int* p = new int;
		printf ("3 called malloc()\n\r");
	}
	heap_diag (true);

	{
		printf ("4 calling malloc()\n\r");
		int* p = new int;
		printf ("4 called malloc()\n\r");
	}
	heap_diag (true);

	{
		printf ("5 calling malloc()\n\r");
		char* p = new char [65500];
		printf ("5 called malloc()\n\r");
	}
	heap_diag (true);

	{
		printf ("6 calling malloc()\n\r");
		char* p = new char [5];
		printf ("6 called malloc()\n\r");
	}
	heap_diag (true);
}

int main ()
{
	//set_heap_trace(2);
	__putstr (banner);	

	unsigned char* vectorBaseAddress = (unsigned char*) &__vector_table[0];
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
	v.setVector (39, &trap7);
	v.setVector (40, &trap8);
	v.setVector (41, &trap9);

	// detailed diagnostics
	printf ("installed TRAPs\n\r");

    unsigned int ticksNow = ticks;
    printf ("waiting for timer to stabilise\n\r");
    while (ticks == ticksNow) 
      ; 
    printf ("timer stabilised at %d\n\r", _zebulon_time ());

	{
		::DiskInfo info;
		unsigned int result = __ide_ident (info, _zebulon_time, _zebulon_time () + diskTimeoutInMS);

	    if (result == ::IDE_OK)
		{
			double capacity = ((double) info.totalNumOfUserSectors) * ide_block_size / 1000000000;
			printf ("identified %s (%d Gb)\n\r", info.modelNumber, (unsigned long) capacity);
			printf (" serial number\t\t: %s\n\r",info.serialNumber);
			printf (" firmware revision\t: %s\n\r",info.firmwareRevision);
		}
		else
      		printf (">>> ident error 0x%x\n\r", result);
	}

	//test_heap ();
	//return 0;

	theFiler ().load ();

	printf ("\n\r");
	heap_diag (false);

	int result = Shell ().run ();

	__disable_interrupts ();
	printf ("byebye\n\r");

	return result;
}
