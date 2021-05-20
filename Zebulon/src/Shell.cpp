#include "Shell.h"
#include <stdio.h>
#include <string.h>
#include <IDE.h>
#include <string>
#include <list>


extern char* __begin;
extern char* __end;
extern char* start;

namespace {

void testString ()
{
	std::string s1 ("HelloWorld");
	printf ("0x%x %s\n\r", s1.c_str (), s1.c_str ());

	std::string s2 (s1);
	printf ("0x%x %s\n\r", s2.c_str (), s2.c_str ());

	s2 = s1;
	printf ("0x%x %s\n\r", s2.c_str (), s2.c_str ());

	if (s1 == s2)
		printf ("same\n\r");
	else
		printf ("different\n\r");
}

void printList (std::list<int>& l)
{
	printf ("-----\n\r");
	for (std::list<int>::iterator i = l.begin (); i != l.end (); i++)
	{
		printf ("%d\n\r",*i);
	}
	printf ("=====\n\r");
}


void testList ()
{
	std::list<int> l;
	printList (l);
	l.push_front (1);
	printList (l);
	l.push_front (2);
	printList (l);
	l.push_front (3);
	printList (l);
}



void printHelp (void)
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
	printf ("ident\t - ident the disk\n\r");
	printf ("write\t - write data to disk\n\r");
	printf ("read\t - read data from disk\n\r");
	printf ("save\t - save code to disk\n\r");
}

void ident ()
{
	IDE i;
	IDE::DiskInfo info;
	if (i.ident (info))
	{
		printf ("model number\t\t\t\t: %s\n\r",info.modelNumber);
		printf ("serial number\t\t\t\t: %s\n\r",info.serialNumber);
		printf ("firmware revision\t\t\t: %s\n\r",info.firmwareRevision);
		printf ("\n\r");

		if (info.fixedDrive) printf ("fixed drive\n\r");
		if (info.removeableCartridgeDrive) printf ("removeable cartridge drive\n\r");
		if (info.hardSectored) printf ("hard sectored\n\r");
		if (info.softSectored) printf ("soft sectored\n\r");
		if (info.LbaSupported) printf ("LBA supported\n\r");
		if (info.DmaSupported) printf ("DMA supported\n\r");
		if (info.notMfmEncoded) printf ("not MFM encoded\n\r");
		if (info.headSwitchTime15uS) printf ("head switch time > 15uS\n\r");
		if (info.transferRateLt5Mbs) printf ("disk transfer rate < 5Mbs\n\r");
		if (info.transferRateLt10Mbs) printf ("disk transfer rate > 5Mbs buyt <- 10Mbs\n\r");
		if (info.transferRateGt10Mbs) printf ("disk transfer rate > 10Mbs\n\r");
		if (info.doubleWordIO) printf ("can perform double word io\n\r");
		if (info.spindleMotorControlOptionImplemented) printf ("spindle motor control option implemented\n\r");
		if (info.dataStrobeOffsetOptionAvailable) printf ("data strobe offset option availabe\n\r");
		if (info.trackOffsetOptionAvailable) printf ("track offset option available\n\r");
		if (info.formatSpeedToleranceGapRequired) printf ("format speed tolerance gap required\n\r");
		printf ("\n\r");

		printf ("number of cylinders\t\t\t: %d\n\r",info.numCylinders);
		printf ("number of heads\t\t\t\t: %d\n\r",info.numHeads);
		printf ("bytes per track\t\t\t\t: %d\n\r",info.numBytesPerTrack);
		printf ("bytes per sector\t\t\t: %d\n\r",info.numBytesPerSector);
		printf ("sectors per track\t\t\t: %d\n\r",info.numSectorsPerTrack);
		printf ("\n\r");

		printf ("buffer type\t\t\t\t: %x\n\r",info.bufferType);
		printf ("buffer size\t\t\t\t: %d\n\r",info.bufferSize);
		printf ("number of ECC bytes\t\t\t: %d\n\r",info.numEccBytes);
		printf ("max rw sectors per interrupt\t\t: %x\n\r",info.maxRwSectorsPerInterrupt);
		printf ("\n\r");

		printf ("PIO mode\t\t\t\t: %d\n\r",info.PioMode);
		printf ("DMA mode\t\t\t\t: %d\n\r",info.DmaMode);	
		printf ("\n\r");

		printf ("number of current cylinders\t\t: %d\n\r",info.numCurrentCylinders);
		printf ("number of current heads\t\t\t: %d\n\r",info.numCurrentHeads);
		printf ("number of current sectors per track\t: %d\n\r",info.numCurrentSectorsPerTrack);
		printf ("current capacity in sectors\t\t: %d\n\r",info.currentCapacityInSectors);
		printf ("total number of user sectors\t\t: %d\n\r",info.totalNumOfUserSectors);
		printf ("\n\r");
		
		printf ("single word DMA modes supported\t\t: %d\n\r",info.singlewordDmaModesSupported);
		printf ("single word DMA modes active\t\t: %d\n\r",info.singlewordDmaModesActive);
		printf ("multi word DMA modes supported\t\t: %d\n\r",info.multiwordDmaModesSupported);
		printf ("multi word DMA modes active\t\t: %d\n\r",info.multiwordDmaModesActive);
	}
	else
		printf ("ERROR: ident failed\n\r");
}

const unsigned long lba = 512;

void write ()
{
	unsigned char data [512] = "The house stood on a slight rise just on the edge of the village. It stood on its own and looked out over a broad spread of West Country farmland. Not a remarkable house by any means—it was about thirty years old, squattish, squarish, made of brick, and had four windows set in the front of a size and proportion which more or less exactly failed to please the eye.  The only person for whom the house was in any way special was Arthur Dent, and that was only because it happened to be the one he lived in.";
	
	IDE i;
	i.write (lba, data);
}


void read ()
{
	unsigned char data [512];
	
	IDE i;
	i.read (lba, data);

	printf ("%s\n\r",data);
}

void save ()
{
	IDE i;

	const int startBlock = 513;
	int curBlock = startBlock;


	static char* begin = (char*) &__begin;
	static char* end = (char*) &__end;
	static char* entry = (char*) &start;

	printf ("start 0x%x end 0x%x entry 0x%x\n\r", begin, end, entry);

	size_t length = end - begin;
	size_t numBlocks = (length / 512) + 1;

	printf ("%d bytes, which is %d blocks\n\r", length, numBlocks);

	for (int b = 0; b < length;)
	{
		printf ("b %d\n\r", b);

		unsigned char block [512];
		if (b == 0)
		{
			memcpy (&block, &begin, 4);
			memcpy (&block [4], &end, 4);
			memcpy (&block [8], &entry, 4);
			memcpy (&block [12], begin, 500);
			b += 500;
		}
		else
		{
			memcpy (&block, begin + b, 512);
			b += 512;
		}

		printf ("writing block %d\n\r", curBlock);
		i.write (curBlock, block);
		curBlock++;
	}
}

}


Shell::Shell (unsigned int& tick) : m_tick (tick)
{
}

void Shell::run () const
{
	const char* version = "Z-Shell V1.10";
	printf ("%s\n\r",version);

	printf ("type help for help\n\r");

	char buf [21];
	int exit = 0;
	while (!exit)
	{
		printf ("$ ");
		char* p = gets (buf);
		*p = '\0';
		printf ("\n\r");

		if (strcmp (buf, "exit") == 0) exit = 1;
		if (strcmp (buf, "version") == 0) printf ("%s\n\r",version);
		if (strcmp (buf, "help") == 0) printHelp ();
		if (strcmp (buf, "ident") == 0) ident ();
		if (strcmp (buf, "read") == 0) read ();
		if (strcmp (buf, "write") == 0) write ();
		if (strcmp (buf, "save") == 0) save ();
		if (strcmp (buf, "uptime") == 0) printf ("uptime: %d\n\r", m_tick);
		if (strcmp (buf, "tststr") == 0) testString ();
		if (strcmp (buf, "tstlst") == 0) testList ();
		}

	return ;
}
