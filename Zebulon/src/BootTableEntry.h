#ifndef BOOTTABLEENTRY_H
#define BOOTTABLEENTRY_H

#include "block_address_t.h"
#include <shared_ptr>
#include <string>
#include <string.h>
#include <size_t.h>

struct BootTableEntry
{
	typedef mdf::shared_ptr <BootTableEntry> Ptr;
	typedef mdf::shared_ptr <const BootTableEntry> ConstPtr;

	BootTableEntry () : empty (true) {}
	BootTableEntry (const std::string& name, unsigned int _index,
		unsigned int _length, unsigned int _loadAddress, 
		unsigned int _goAddress, block_address_t _startBlock)
		: index (_index), length (_length), loadAddress (_loadAddress),
		goAddress (_goAddress), startBlock (_startBlock), empty (false)
	{
		size_t len = name.length ();
		if (len > 20) len = 20;
		memcpy (shortName, name.c_str (), len + 1);
		shortName[20] = '\0';
	}

	bool empty;					
	char shortName [21];
	unsigned int index;
	unsigned int length;
	unsigned int loadAddress;
	unsigned int goAddress;
	block_address_t startBlock;
};

#endif
