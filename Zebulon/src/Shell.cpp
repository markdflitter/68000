#include "../private_include/Shell.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ZebulonAPI.h>
#include "../private_include/Utils.h"
#include "../private_include/version.h"
#include <timer.h>
#include <algorithm>
#include "../private_include/CommandReader.h"

using namespace std;
using namespace Zebulon;
using namespace mdf;

extern char __begin[];
extern char __end[];
extern char start[];

namespace {

unsigned long printBuffer (unsigned char* buffer, size_t bufferLen, unsigned long startAddress, bool hexAddress)
{
	unsigned char* p = buffer;
	
	unsigned long address = startAddress;

	while (bufferLen > 0)
	{
		size_t rowLen = min (bufferLen, 16);

		if (hexAddress)
			printf ("0x%x\t : ", address);
		else
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

void printStats (const string& filename, const _zebulon_stats& stats)
{
	printf ("%s : %d byte(s), allocated %d\n\r", Utils::padFilename (filename).c_str (), stats.size, stats.sizeOnDisk);
}

}	

namespace {

void printVersion ()
{
	printf ("%s (built on %s at %s)\n\r", VERSION, BUILD_DATE, BUILD_TIME);
}

void printHelp (void)
{
	printf ("version\t\t\t\t - print version\n\r");
	printf ("help\t\t\t\t - print this help\n\r");
	printf ("exit\t\t\t\t - exit to monitor\n\r");
	printf ("restart\t\t\t\t - restart the system\n\r");
	printf ("display <num>\t\t\t - display a number\n\r");
	printf ("history show\t\t\t - show command history\n\r");
	printf ("history clear\t\t\t - clear command history\n\r");
	printf ("history <n>\t\t\t - fetch history item\n\r");
	printf ("diag filer\t\t\t - print filer diagnostics\n\r");
	printf ("diag heap {full}\t\t - print (full) heap diagnostics\n\r");
	printf ("memory dump <start> <bytes>\t - print memory dump\n\r");
	printf ("uptime\t\t\t\t - print time since boot\n\r");
	printf ("disk ident\t\t\t - ident the disk and print data\n\r");
	printf ("disk read <block>\t\t - read specified block from disk\n\r");
	printf ("disk write <block> <pattern>\t - write pattern to specified block on disk\n\r");
	printf ("disk soak\t\t\t - soak test the disk\n\r");
	printf ("filer format\t\t\t - format the filing system\n\r");
	printf ("filer diag\t\t\t - print filing system diagnostics\n\r");
	printf ("filer space\t\t\t - print filing system free space\n\r");
	printf ("filer ls\t\t\t - list files\n\r");
	printf ("filer index\t\t\t - print boot table\n\r");
	printf ("filer file read <filename>\t - read file\n\r");
	printf ("filer file write <filename>\t - write file\n\r");
	printf ("filer file stat <filename>\t - stat file\n\r");
	printf ("filer file delete <filename>\t - delete file\n\r");
	printf ("save <filename> <boot slot>\t - save file and make it bootable\n\r");
}

void diag_heap (bool full)
{
	_zebulon_heap_diag (full);
}

void dump_memory (unsigned int start, unsigned int length)
{
	unsigned char* p = (unsigned char*) start;
	printBuffer (p, length, (unsigned long) p, true);
	printf ("\n\r");
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
		printBuffer (buffer, 512, 0, false);
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
	unsigned int block = 0;
	t = t - 10;
	while (true)
	{
		if (clock () / 1000 > t + 10)	
		{
			disk_read (block);
			t = clock () / 1000;
			printf ("\n\r");
	
			diag_heap (false);

			printf ("\n\r");

			block = (block + 1);
			if (block == 100) block = 0;
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

void stat_file (const string& filename);
void ls_filer ()
{
	char buffer [FILENAME_BUFFER_SIZE];

	int sh = _zebulon_find_first_file (buffer);
	
	int numFiles = 0;
	while (sh > -1)
	{
		stat_file (buffer);
	
		numFiles++;
		bool result = _zebulon_find_next_file (sh, buffer);
		if (!result) 
		{
			_zebulon_find_close (sh);
			break;
		}
	}
	printf (" %d file(s)\n\r", numFiles);
}

void index_filer ()
{
	_zebulon_boot_table_entry btes [9];
	_zebulon_index (btes);

	for (int i = 0; i < 9; i++)
	{
	if (!btes[i].empty)
		{
			printf ("%d : ", i);
			printf ("%s len = %d byte(s), load 0x%x, start = 0x%x, firstBlock = %d\n\r", Utils::pad (btes[i].name, ' ', 20).c_str (), btes[i].length, btes[i].loadAddress, btes[i].startAddress, btes[i].startBlock);
		}
	}
}

void read_file (const string& filename)
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
		address = printBuffer (buffer, bytesRead, address, false);
	}

	fclose (f);
}

void write_file (const string& filename, unsigned long bytes)
{
	printf ("writing %d bytes to file '%s'\n\r", bytes, filename.c_str ());
	
	FILE* f = fopen (filename.c_str (), "wb");
	if (f == 0) return ;

	unsigned long bytesLeftToWrite = bytes;

	char data [] = "Marley was dead: to begin with. There is no doubt whatever about that. The register of his burial was signed by the clergyman, the clerk, the undertaker, and the chief mourner. Scrooge signed it. And Scrooge's name was good upon 'Change, for anything he chose to put his hand to. Old Marley was as dead as a door-nail. Mind! I don't mean to say that I know, of my own knowledge, what there is particularly dead about a door-nail. I might have been inclined, myself, to regard a coffin-nail as the deadest piece o";
	size_t len = strlen (data);

	unsigned long buffers = bytesLeftToWrite / len;
	if (bytesLeftToWrite % len > 0) buffers++;

	unsigned long buffer = 0;
	while (bytesLeftToWrite > 0)
	{
		unsigned long bytesThisTime = min (bytesLeftToWrite, len);

		fwrite ((unsigned char*) data, 1, bytesThisTime, f);
		bytesLeftToWrite -= bytesThisTime;

		buffer++;
		if (buffers > 1) printf (" buffer %d / %d\n\r", buffer, buffers);
	}

	fclose (f);
}

void stat_file (const string& filename)
{
	_zebulon_stats stats;
	if (_zebulon_stat_file (filename.c_str  (), &stats) == 0)
		printStats (filename, stats);
}

void delete_file (const string& filename)
{
	if (_zebulon_delete_file (filename.c_str ()))
		printf ("deleted file %s\n\r", filename.c_str ());
}

void save (const string& filename, unsigned char bootslot)
{
	if (bootslot > 9)
	{
		printf (">> max boot slot is %d, got %d\n\r", 9, bootslot);
		return ;
	}

	printf ("saving to file '%s' in boot slot %d\n\r", filename.c_str (), bootslot);

	static unsigned char* loadAddress = (unsigned char*) &__begin[0];
	static unsigned char* end = (unsigned char*) &__end[0];
	static unsigned char* startAddress = (unsigned char*) &start[0];

	printf (" start 0x%x end 0x%x start 0x%x\n\r", loadAddress, end, startAddress);

	unsigned long length = end - loadAddress;
	unsigned long blocks = length / 512;
	if ((length % 512) != 0) blocks++;
	
	if (Utils::file_exists (filename))
		if (_zebulon_delete_file (filename.c_str ()))
			printf ("deleted file %s\n\r", filename.c_str ());

	if (!_zebulon_create_file (filename.c_str (), blocks, true))
	{
		printf (">>> failed to create file %s of size %d byte(s), %d block(s)\n\r", filename.c_str (), length, blocks);
		return;
	}

	FILE* f = fopen (filename.c_str (), "wb");
	if (f == 0) return ;

	unsigned long bytesLeftToWrite = length;
	unsigned char* p = loadAddress;

	timer t;
	while (bytesLeftToWrite > 0)
	{
		unsigned long bytesThisTime = min (bytesLeftToWrite, 512);

		fwrite (p, 1, bytesThisTime, f);
		bytesLeftToWrite -= bytesThisTime;
		p += bytesThisTime;
		printf (".");
	}
	printf (" %dmS\n\r", t.elapsed ());

	if (!_zebulon_boot_boot (bootslot, filename.c_str (), (unsigned int) loadAddress, (unsigned int) startAddress, length))
		printf (">>> error creating boot table entry\n\r");
	
	fclose (f);
}

}

int Shell::run () const
{
	printf ("\n\r");
	printVersion();
	printf ("type help for help\n\r\n\r");

	char buf [255];
	int exit = 0;
	int result = 0;

	CommandReader commandReader;
	commandReader.loadHistory ();

	printf ("$ ");
	
	while (!exit)
	{
		string command = commandReader.read ();
		printf ("\n\r");
		vector<string> tokens = Utils::tokenize (command);

		if (tokens.size () > 0)
		{
			if (tokens [0] == "version") printVersion ();
			if (tokens [0] == "help") printHelp ();
			if (tokens [0] == "exit") exit = 1;
			if (tokens [0] == "restart") 
			{
				exit = 1;
				result = 1;
			}
			if (tokens [0] == "display" && tokens.size () > 1) 
			{
				unsigned int num = atol (tokens [1].c_str ());
				_zebulon_set_display (num);
			}
			if (tokens [0] == "diag" && tokens.size () > 1) 
			{
				if (tokens [1] == "heap") 
				{
					bool full = false;
					if (tokens.size () > 2 && tokens [2] == "full") full = true;
					diag_heap (full);
				}
				if (tokens [1] == "filer") diag_filer ();
			}
			if (tokens [0] == "memory" && tokens.size () > 3)
			{
				if (tokens [1] == "dump")
				{
					unsigned int address  = atol (tokens [2].c_str ());
					unsigned int length = atol (tokens [3].c_str ());
					dump_memory (address, length);
				}
			}
			if (tokens [0] == "history" && tokens.size () > 1)
			{
				if (tokens [1] == "show") commandReader.showHistory ();
				else if (tokens [1] == "clear") commandReader.clearHistory ();
				else {
					printf ("$ ");
					unsigned int item  = atol (tokens [1].c_str ());
					commandReader.loadHistoryItem (item);
					continue ;
				}
			}
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
				if (tokens [1] == "ls")
				{
					ls_filer ();
				}
				if (tokens [1] == "index")
				{
					index_filer ();
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
							unsigned long bytes = (unsigned long) atol (tokens [4].c_str ());
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
			if (tokens [0] == "save" && tokens.size () > 2)
			{
				string filename = tokens [1];
				unsigned char bootslot = atol (tokens [2].c_str ());
				save (filename, bootslot);
			}
		}

		printf ("$ ");	
	}
	return result;
}
