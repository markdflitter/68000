#include "Shell.h"
#include <stdio.h>
#include <string.h>
#include <bsp.h>
#include <string>
#include <list>
#include <vector>
#include <stdlib.h>
#include "FAT.h"
#include "ctype.h"

extern char* __begin;
extern char* __end;
extern char* start;

namespace {

std::vector<std::string> tokenize (const std::string& command)
{
	std::vector<std::string> result;

	const char* str = command.c_str ();
	char buf [255];
	char* cur = buf;
	while (*str != '\0')
	{
		if (*str != ' ')
			*cur++ = *str;
		else
		{
			*cur = '\0';
			result.push_back (std::string (buf));
			cur = buf;
		}
		str++;
	}

	if (cur != buf)
	{
		*cur = '\0';
		result.push_back (std::string (buf));
		cur = buf;
	}

	return result;
}



void format (FAT& fat, unsigned long size)
{
	printf ("formatting to %d\n\r", size);	
	fat.format (size);
}


void statFile (const FAT::File& file)
{
	printf ("%s : ",file.m_name.c_str ());
	for (std::list<SpaceManager::Chunk>::const_iterator i = file.m_chunks.begin (); i != file.m_chunks.end (); i++)
		printf ("%d -> %d\n\r", (*i).m_start, (*i).m_start + (*i).m_length);
}

void createFile (FAT& fat, const std::string& filename, unsigned long size)
{
	FAT::File file1 = fat.createFile (filename, size);
	statFile (file1);
}

void ls (const FAT& fat)
{
	std::list<FAT::File> files = fat.ls ();
	for (std::list<FAT::File>::iterator i = files.begin (); i != files.end (); i++)
		statFile (*i);
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
	DiskInfo info;
	if (__ide_ident (info))
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

void write (unsigned long block)
{
	printf ("writing to %d\n\r", block);

	unsigned char data [512] = "The house stood on a slight rise just on the edge of the village. It stood on its own and looked out over a broad spread of West Country farmland. Not a remarkable house by any means—it was about thirty years old, squattish, squarish, made of brick, and had four windows set in the front of a size and proportion which more or less exactly failed to please the eye.  The only person for whom the house was in any way special was Arthur Dent, and that was only because it happened to be the one he lived in.";
	
	__ide_write (block, data);
}

void read (unsigned long block)
{
	printf ("reading from %d\n\r", block);

	unsigned char data [512];
	
	__ide_read (block, data);

	unsigned char* p = data;

	for (int l = 0; l < 32; l++)
	{
		for (int c = 0; c < 16; c++) printf ("%x",*p++);
		printf (" ");

		p -= 16;
		for (int c = 0; c < 16; c++)
		{
			if (isprint (*p))
				printf ("%c",*p);
			else
				printf (".");
			p++;
		}
		printf ("\n\r");
	}
}

void save ()
{
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
		__ide_write (curBlock, block);
		curBlock++;
	}
}

}


Shell::Shell (unsigned int& tick) : m_tick (tick)
{
}

void Shell::run () const
{
	const char* version = "Z-Shell V1.11";
	printf ("%s\n\r",version);

	FAT fat;

	printf ("type help for help\n\r");

	char buf [255];
	int exit = 0;
	while (!exit)
	{
		printf ("$ ");
		char* p = gets (buf);
		*p = '\0';
		printf ("\n\r");

		std::string command (buf);
		std::vector<std::string> tokens = tokenize (command);

		if (tokens.size () > 0)
		{
			if (tokens [0] == "exit") exit = 1;
			if (tokens [0] == "version") printf ("%s\n\r",version);
			if (tokens [0] == "help") printHelp ();
			if (tokens [0] == "ident") ident ();
			if (tokens [0] == "read" && tokens.size () > 1) 
			{
				unsigned long block = atol (tokens [1].c_str ());
				read (block);
			}
			if (tokens [0] == "write" && tokens.size () > 1)
			{
				unsigned long block = atol (tokens [1].c_str ());
				write (block);
			}
			if (tokens [0] == "save") save ();
			if (tokens [0] == "create" && tokens.size () > 2)
			{
				std::string filename (tokens [1].c_str ());
				unsigned long size = atol (tokens [2].c_str ());
				createFile (fat, filename, size);
			}
			if (tokens [0] == "ls") ls (fat);
			if (tokens [0] == "format" && tokens.size () > 1)
			{
				unsigned long size = atol (tokens [1].c_str ());			
				format (fat, size);
			}			
		}
	}

	return ;
}
