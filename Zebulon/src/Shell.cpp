#include "Shell.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ZebulonAPI.h>
#include "Utils.h"

const char* version = "Z-Shell V2.00.0004";
const char* filename = "Zebulon_V2.0.0004";

using namespace std;
using namespace Zebulon;

namespace {

vector<string> tokenize (const string& command)
{
	vector<string> result;

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
			result.push_back (string (buf));
			cur = buf;
		}
		str++;
	}

	if (cur != buf)
	{
		*cur = '\0';
		result.push_back (string (buf));
		cur = buf;
	}

	return result;
}

unsigned long printBuffer (unsigned char* buffer, size_t bufferLen, unsigned long startAddress)
{
	unsigned char* p = buffer;
	
	unsigned long address = startAddress;

	while (bufferLen > 0)
	{
		size_t rowLen = Utils::min (bufferLen, 16);

		printf ("%d\t : ", address);
		address += rowLen;

		for (int c = 0; c < rowLen; c++) 
		{
			if (c != 0) printf (" ");
			printf ("%x",*p++);
		}

			
		string pad (3 * (16 - rowLen) + 4, ' ');	
		printf ("%s", pad.c_str ());

		p -= rowLen;
		for (int c = 0; c < rowLen; c++)
		{
			if (isprint (*p))
				printf ("%c",*p);
			else
				printf (".");
			p++;
		}
		bufferLen -= rowLen;
		printf ("\n\r");
	}

	return address;
}

void printStats (const std::string& filename, const _zebulon_stats stats)
{
	string pad (64 - filename.length (), ' ');	

	printf ("%s%s : %d byte(s), allocated %d\n\r", filename.c_str (), pad.c_str (), stats.size, stats.sizeOnDisk);
}

}	

namespace {

void printHelp (void)
{
	printf ("version\t\t\t\t - print version\n\r");
	printf ("help\t\t\t\t - print this help\n\r");
	printf ("exit\t\t\t\t - exit to monitor\n\r");
	printf ("uptime\t\t\t\t - print time since boot\n\r");
	printf ("disk ident\t\t\t - ident the disk and print data\n\r");
	printf ("disk read <block>\t\t - read specified block from disk\n\r");
	printf ("disk write <block> <pattern>\t - write pattern to specified block on disk\n\r");
	printf ("disk soak\t\t\t - soak test the disk\n\r");
	printf ("filer format\t\t\t - format the filing system\n\r");
	printf ("filer diag\t\t\t - print filing system diagnostics\n\r");
	printf ("filer space\t\t\t - print filing system free space\n\r");
	printf ("filer file read <filename>\t - read file\n\r");
	printf ("filer file write <filename>\t - write file\n\r");
	printf ("filer file stat <filename>\t - stat file\n\r");
	printf ("filer file delete <filename>\t - delete file\n\r");
}

}


void uptime ()
{
	printf ("%d seconds\n\r", clock () / 1000);
}

void disk_ident ()
{
	Zebulon::DiskInfo info;
	unsigned int result = _zebulon_ide_ident (info);
	if (result == Zebulon::IDE_OK)
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
		printf (">>> ident error 0x%x\n\r", result);
}


void disk_read (unsigned long block)
{
	printf ("reading block %d\n\r", block);

	unsigned char buffer [512];
	memset (buffer, '\0', 512);

	unsigned int result = _zebulon_ide_read_block (block, buffer);
	if (result == Zebulon::IDE_OK)
		printBuffer (buffer, 512, 0);
	else
		printf (">>> read error 0x%x\n\r", result);
}

void disk_write (unsigned long block, unsigned char pattern)
{
	printf ("writing block %d\n\r", block);

	unsigned char buffer [512];
	memset (buffer, pattern, 512);

	unsigned int result = _zebulon_ide_write_block (block, buffer);
	if (result == Zebulon::IDE_OK)
		printf ("OK\n\r");
	else
		printf (">>> write error 0x%x\n\r", result);
}

void disk_soak ()
{
	unsigned long t = clock () / 1000;
	t = t - 10;
	while (true)
	{
		if (clock () / 1000 > t + 10)	
		{
			disk_read (1);
			t = clock () / 1000;
			printf ("\n\r");
		}
	}
}

void format_filer ()
{
	printf ("formatting...");

	int result = _zebulon_filer_format ();
	if (result >= 0)
	{
		printf ("OK\n\r");
		printf ("chunks = %d\n\r",result);
	}
	else
		printf ("Failed\n\r");
}

void diag_filer ()
{
	_zebulon_filer_diag ();
}


void free_space_filer ()
{
	_zebulon_free_space fs = _zebulon_filer_free_space ();

	printf ("total free: %d out of %d (%d%%)\n\r", fs.freeSpace, fs.totalSpace, ((unsigned int) (100 * double(fs.freeSpace) / fs.totalSpace)));
}

void read_file (const std::string& filename)
{
	printf ("reading file '%s'\n\r", filename.c_str ());

	FILE* f = fopen (filename.c_str (), "rb");
	if (f == 0) return ;
	
	unsigned long address = 0;	
	while (!feof (f))
	{
		unsigned char buffer [512];
		memset (buffer, 0, 512);

		unsigned int bytesRead = fread (buffer, 1, 512, f);
		address = printBuffer (buffer, bytesRead, address);
	}

	fclose (f);
}

void write_file (const std::string& filename, unsigned long bytes)
{
	printf ("writing %d bytes to file '%s'\n\r", bytes, filename.c_str ());
	
	FILE* f = fopen (filename.c_str (), "wb");
	if (f == 0) return ;

	unsigned long bytesLeftToWrite = bytes;

	char data [] = "Marley was dead: to begin with. There is no doubt whatever about that. The register of his burial was signed by the clergyman, the clerk, the undertaker, and the chief mourner. Scrooge signed it. And Scrooge's name was good upon 'Change, for anything he chose to put his hand to. Old Marley was as dead as a door-nail. Mind! I don't mean to say that I know, of my own knowledge, what there is particularly dead about a door-nail. I might have been inclined, myself, to regard a coffin-nail as the deadest piece of ironmongery in the trade. But the wisdom of our ancestors is in the simile; ";
	size_t len = strlen (data);

	while (bytesLeftToWrite > 0)
	{
		unsigned long bytesThisTime = Utils::min (bytesLeftToWrite, len);

		fwrite ((unsigned char*) data, 1, bytesThisTime, f);
		bytesLeftToWrite -= bytesThisTime;
	}

	fclose (f);
}

void stat_file (const std::string& filename)
{
	_zebulon_stats stats = _zebulon_stat_file (filename.c_str  ());
	printStats (filename, stats);
}

void delete_file (const std::string& filename)
{
	if (_zebulon_delete_file (filename.c_str ()))
		printf ("deleted file %s\n\r", filename.c_str ());
}

void Shell::run () const
{
	printf ("\n\r%s\n\r",version);
	printf ("type help for help\n\r\n\r");

	char buf [255];
	int exit = 0;
	while (!exit)
	{
		printf ("$ ");
		char* p = gets (buf);

		*p = '\0';
		printf ("\n\r");
		string command (buf);
		vector<string> tokens = tokenize (command);

		if (tokens.size () > 0)
		{
			if (tokens [0] == "version") printf ("%s\n\r",version);
			if (tokens [0] == "help") printHelp ();
			if (tokens [0] == "exit") exit = 1;
			if (tokens [0] == "uptime") uptime ();
			if (tokens [0] == "disk" && tokens.size () > 1) 
			{
				if (tokens [1] == "ident")
				{
					disk_ident ();				
				}
				if (tokens [1] == "soak")
				{
					disk_soak ();				
				}
				else if (tokens.size () > 2)
				{
					unsigned long block = atol (tokens [2].c_str ());
					if (tokens [1] == "read")
					{
						disk_read (block);				
					}
					if (tokens [1] == "write" && tokens.size () > 3)
					{
						unsigned char pattern = (unsigned char) atol (tokens [3].c_str ());
						disk_write (block, pattern);
					}
				}
			}
			if (tokens [0] == "filer" && tokens.size () > 1)
			{
				if (tokens [1] == "format")
				{
					format_filer ();
				}
				if (tokens [1] == "diag")
				{
					diag_filer ();
				}
				if (tokens [1] == "space")
				{
					free_space_filer ();
				}
				if (tokens.size () > 3)
				{
					string filename = tokens [3];
					if (tokens [1] == "file")
					{
						if (tokens [2] == "read")
						{
							read_file (filename);
						}
						if (tokens [2] == "write" && tokens.size () > 4)
						{
							unsigned char bytes = (unsigned char) atol (tokens [4].c_str ());
							write_file (filename, bytes);
						}
						if (tokens [2] == "stat")
						{
							stat_file (filename);
						}
						if (tokens [2] == "delete")
						{
							delete_file (filename);
						}
					}
				}
			}
		}

	}
	return ;
}
