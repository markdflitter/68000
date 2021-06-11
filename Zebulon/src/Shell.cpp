#include "Shell.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <list>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <timer>
#include "FAT.h"
#include <ctype.h>
#include <filer.h>

const char* version = "Z-Shell V1.36.0015";
const char* filename = "Zebulon_V1.36.0015";
	
using namespace std;

extern char* __begin;
extern char* __end;
extern char* start;


namespace {

string pad (const std::string& str, size_t totalLength, char c)
{
	string result = str;
	result.resize (totalLength, c);
	return result;
}

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

}

namespace {

void printHelp (void)
{
	printf ("version\t\t\t\t - print version\n\r");
	printf ("help\t\t\t\t - print this help\n\r");
	printf ("exit\t\t\t\t - exit to monitor\n\r");
	printf ("readB <block>\t\t\t - read block from disk\n\r");
	printf ("format <size>\t\t\t - format the filing system\n\r");
	printf ("ls\t\t\t\t - list files\n\r");
	printf ("rm <name>\t\t\t - delete a file\n\r");
	printf ("write <name> <bytes>\t\t - fill file with stuff\n\r");
	printf ("read <name>\t\t\t - read file from disk\n\r");
	printf ("save <bootNumber>\t\t - save code to disk\n\r");
	printf ("unboot <bootNumber>\t\t - empty boot slot\n\r");
	printf ("boot <file> <bootNumber>\t - make file bootable\n\r");
	printf ("time\t\t\t\t - print ticks since boot\n\r");
}

size_t min (size_t l, size_t r)
{
	return l < r ? l : r;
}

void printBuffer (unsigned char* buffer, size_t bufferLen)
{
	unsigned char* p = buffer;

	while (bufferLen > 0)
	{
		size_t rowLen = min (bufferLen, 16);
		for (int c = 0; c < rowLen; c++) printf ("%x",*p++);
	
		string pad (2 * (16 - rowLen) + 1, ' ');	
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
}


void readB (FAT& fat, block_address_t block)
{
	printf ("reading from %d\n\r", block);

	unsigned char data [fat.blockSize ()];
	
	if (fat.readBlock (block, data))
		printBuffer (data, fat.blockSize ());
}

void printStat (struct mdf::stat s)
{
	printf ("%d : %s : ", s.index, pad (s.name, 20, ' ').c_str ());

	if (s.bootable)
		printf ("b");
	else
		printf (" ");
	printf (" : ");	

	printf ("%d bytes\t : %d allocated : 0x%x : 0x%x", s.size, s.sizeOnDisk, s.loadAddress, s.goAddress);

	printf ("\n\r");
}

void save (const std::string& name, unsigned int bootNumber)
{
	mdf::save (name, bootNumber);

/*
	static unsigned char* loadAddress = (unsigned char*) &__begin;
	static unsigned char* end = (unsigned char*) &__end;
	static unsigned char* goAddress = (unsigned char*) &start;

	printf ("saving code: start 0x%x end 0x%x entry 0x%x\n\r", loadAddress, end, goAddress);

	file_address_t length = end - loadAddress;
	block_address_t numBlocks = (length / fat.blockSize ()) + 1;

	fat.rm (name);
	if (fat.create (name, numBlocks))
	{
		struct stat s;
		stat (name, &s);
		printStat (s);
	}
	else
		return ;

	fat.setMetaData (name, (unsigned int) loadAddress, (unsigned int) goAddress);

	file_handle f = fat.open (name);
	if (f == file_not_found) return ;

	file_address_t bytesLeftToWrite = length;

	unsigned char* p = loadAddress;

	timer t;
	while (bytesLeftToWrite > 0)
	{
		unsigned char buffer [512];
		if (bytesLeftToWrite >= 512)
		{
			memcpy (buffer, p, 512);
			fat.write (f, buffer, 512);
			bytesLeftToWrite -= 512;
			p += 512;
		}
		else
		{
			memcpy (buffer, p, bytesLeftToWrite);
			fat.write (f, buffer, bytesLeftToWrite);
			bytesLeftToWrite -= bytesLeftToWrite;
		}

		printf (".");
	}

	printf (" %dmS\n\r", t.elapsed ());

	fat.close (f);

	fat.unboot (bootNumber);
	fat.boot (name, bootNumber);
*/
}

void unboot (unsigned int bootNumber)
{
	printf ("clearing boot file %d\n\r", bootNumber);
	mdf::unboot (bootNumber);
}

void boot (const string& filename, unsigned int bootNumber)
{
	printf ("making file '%s' bootable from slot %d\n\r", filename.c_str (), bootNumber);
	mdf::boot (filename, bootNumber);
}

void index ()
{
	mdf::bootTableEntry btes [10];
	mdf::index (btes);

	for (int i = 0; i < 10; i++)
	{
		if (!btes [i].empty)
			printf ("%d : %s : %d : 0x%x : 0x%x : 0x%x : %d\n\r", 
				btes[i].index, btes[i].name, btes[i].file_index, btes[i].size, btes[i].loadAddress, btes[i].goAddress, btes[i].startBlock);
	}
}
	
void format (block_address_t size)
{
	printf ("formatting: size %d blocks\n\r", size);	
	mdf::format (size);
}

void rmFile (const string& filename)
{
	printf ("deleting file '%s'\n\r", filename.c_str ());
	mdf::deleteFile (filename);
}

void write (const string& filename, block_address_t size)
{
	printf ("writing %d bytes to file '%s'\n\r", size, filename.c_str ());
	
	FILE* f = fopen (filename.c_str (), "wb");
	if (f == 0) return ;

	file_address_t bytesLeftToWrite = size;

	unsigned char data [] = "Marley was dead: to begin with. There is no doubt whatever about that. The register of his burial was signed by the clergyman, the clerk, the undertaker, and the chief mourner. Scrooge signed it. And Scrooge's name was good upon 'Change, for anything he chose to put his hand to. Old Marley was as dead as a door-nail. Mind! I don't mean to say that I know, of my own knowledge, what there is particularly dead about a door-nail. I might have been inclined, myself, to regard a coffin-nail as the deadest piece of ironmongery in the trade. But the wisdom of our ancestors is in the simile;           ";

	unsigned char* p = data;

	while (bytesLeftToWrite > 0)
	{
		unsigned char buffer [100];
		if (bytesLeftToWrite >= 100)
		{
			memcpy (buffer, p, 100);
			fwrite (buffer, 1, 100, f);
			bytesLeftToWrite -= 100;
			p += 100;
			if (p - data >= 600) p = data;
		}
		else
		{
			memcpy (buffer, p, bytesLeftToWrite);
			fwrite (buffer, 1, bytesLeftToWrite, f);
			bytesLeftToWrite -= bytesLeftToWrite;
		}
	}

	fclose (f);
}

void read (const string& filename)
{
	printf ("reading file '%s'\n\r", filename.c_str ());

	FILE* f = fopen (filename.c_str (), "rb");
	if (f == 0) return ;
	
	printf ("hello\n\r");

	struct mdf::stat s;
	
	if (!mdf::stat (filename, &s))
	{
		fclose (f);
		return ;
	}	

	printf ("byebye\n\r");
	file_address_t bytesLeftToRead = s.size;
	printf ("bytesLeftToRead %d\n\r", bytesLeftToRead);

	while (!feof (f))
	{
		unsigned char buffer [480];
		if (bytesLeftToRead >= 480)
		{
			fread (buffer, 1, 480, f);
			printBuffer (buffer, 480);
			bytesLeftToRead -= 480;
		}
		else
		{
			fread (buffer, 1, bytesLeftToRead, f);
			printBuffer (buffer, bytesLeftToRead);
			bytesLeftToRead -= bytesLeftToRead;
		}	
	}

	fclose (f);
}

void ls ()
{
	struct mdf::stat s;
	int sh = mdf::findFirstFile (&s);

	while (sh > -1)
	{
		printStat (s);
		bool result = mdf::findNextFile (sh, &s);
		if (!result) break;
	}

	mdf::closeFind (sh);
}

void time ()
{
	printf ("%d mS\n\r", clock ());
}

}

Shell::Shell (FAT& fat) : m_fat (fat)
{
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
			if (tokens [0] == "readB" && tokens.size () > 1) 
			{
				block_address_t block = atol (tokens [1].c_str ());
				readB (m_fat, block);
			}
			if (tokens [0] == "save" && tokens.size () > 1)
			{
				unsigned long bootNumber = atol (tokens [1].c_str ());
				::save (filename, (unsigned int) bootNumber);
			}
			if (tokens [0] == "boot" && tokens.size () > 2)
			{
				string filename (tokens [1].c_str ());
				unsigned long bootNumber = atol (tokens [2].c_str ());
				::boot (filename, (unsigned int ) bootNumber);
			}
				if (tokens [0] == "unboot" && tokens.size () > 1)
			{
				unsigned long bootNumber = atol (tokens [1].c_str ());
				::unboot ((unsigned int) bootNumber);
			}
			if (tokens [0] == "index")
			{
				::index ();
			}
			if (tokens [0] == "format" && tokens.size () > 1)
			{
				block_address_t size = atol (tokens [1].c_str ());			
				::format (size);
			}
			if (tokens [0] == "rm" && tokens.size () > 1)
			{
				string filename (tokens [1].c_str ());
				rmFile (filename);
			}
			if (tokens [0] == "write" && tokens.size () > 2)
			{
				string filename (tokens [1].c_str ());
				block_address_t size = atol (tokens [2].c_str ());
				write (filename, size);
			}
			if (tokens [0] == "read" && tokens.size () > 1)
			{
				string filename (tokens [1].c_str ());
				read (filename);
			}
			if (tokens [0] == "ls") ls ();
			if (tokens [0] == "time") time ();
			}
	}

	return ;
}

