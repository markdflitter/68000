#include "../include/print.h"
#include <string>
#include <ctype.h>
#include <stdio.h>


using namespace std;

namespace
{

size_t min (size_t l, size_t r)
{
	return l < r ? l : r;
}

}

namespace mdf
{

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


