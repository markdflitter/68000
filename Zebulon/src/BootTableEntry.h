#ifndef BOOTTABLEENTRY_H
#define BOOTTABLEENTRY_H

#include "block_address_t.h"
#include <shared_ptr>
#include <string>
#include <string.h>
#include <size_t.h>
#include <stdio.h>

struct BootTableEntry
{
	typedef mdf::shared_ptr <BootTableEntry> Ptr;
	typedef mdf::shared_ptr <const BootTableEntry> ConstPtr;

	BootTableEntry () : empty (true)
	{
		shortName.resize (20, ' ');
	}
	BootTableEntry (const std::string& name, unsigned int _index,
		unsigned int _length, unsigned int _loadAddress, 
		unsigned int _goAddress, block_address_t _startBlock)
		: shortName (name), index (_index), length (_length), loadAddress (_loadAddress),
		goAddress (_goAddress), startBlock (_startBlock), empty (false)
	{
		shortName.resize (20,' ');
	}

	bool empty;					
	std::string shortName;
	unsigned int index;
	unsigned int length;
	unsigned int loadAddress;
	unsigned int goAddress;
	block_address_t startBlock;
};

#endif
