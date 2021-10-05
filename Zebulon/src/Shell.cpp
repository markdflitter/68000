#include "Shell.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ZebulonAPI.h>

const char* version = "Z-Shell V2.00.0001";
const char* filename = "Zebulon_V2.0.0001";

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

}	

namespace {

void printHelp (void)
{
	printf ("version\t\t\t\t - print version\n\r");
	printf ("help\t\t\t\t - print this help\n\r");
	printf ("exit\t\t\t\t - exit to monitor\n\r");
	printf ("uptime\t\t\t\t - print time since boot\n\r");
	printf ("read block <block>\t\t - read specified block from disk\n\r");
	printf ("write block <block> <pattern>\t - write pattern to specified block on disk\n\r");
}

void uptime ()
{
	printf ("%d seconds\n\r", clock () / 1000);
}

void read_block (unsigned long block)
{
	printf ("reading block %d\n\r", block);

	unsigned char buffer [512];
	memset (buffer, '\0', 512);

	unsigned int result = _zebulon_read_block (block, buffer);
	if (result == IDE_OK)
		printBuffer (buffer, 512);
	else
		printf (">>> read error 0x%x\n\r", result);
}

void write_block (unsigned long block, unsigned char pattern)
{
	printf ("writing block %d\n\r", block);

	unsigned char buffer [512];
	memset (buffer, pattern, 512);

	unsigned int result = _zebulon_write_block (block, buffer);
	if (result == IDE_OK)
		printf ("OK\n\r");
	else
		printf (">>> write error 0x%x\n\r", result);
}

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
			if (tokens [0] == "read" && tokens.size () > 2) 
			{
				if (tokens [1] == "block")
				{
					unsigned long block = atol (tokens [2].c_str ());
					read_block (block);				
				}
			}
			if (tokens [0] == "write" && tokens.size () > 2) 
			{
				if (tokens [1] == "block" && tokens.size () > 3)
				{
					unsigned long block = atol (tokens [2].c_str ());
					unsigned char pattern = (unsigned char) atol (tokens [3].c_str ());
					write_block (block, pattern);
				}
			}
		}
	}

	return ;
}
