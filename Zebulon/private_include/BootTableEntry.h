#ifndef BOOTTABLEENTRY_H
#define BOOTTABLEENTRY_H

#include <shared_ptr>
#include <string>

namespace Zebulon
{

struct BootTableEntry
{
	typedef mdf::shared_ptr <BootTableEntry> Ptr;
	typedef mdf::shared_ptr <const BootTableEntry> ConstPtr;

	BootTableEntry () : empty (true)
	{
		shortName.resize (20, ' ');
	}
	BootTableEntry (const std::string& name,
		unsigned int _length, unsigned int _loadAddress, 
		unsigned int _startAddress, unsigned long _startBlock)
		: shortName (name), length (_length), loadAddress (_loadAddress),
		startAddress (_startAddress), startBlock (_startBlock), empty (false)
	{
		shortName.resize (20,' ');
	}

	bool empty;					
	std::string shortName;
	unsigned int length;
	unsigned int loadAddress;
	unsigned int startAddress;
	unsigned long startBlock;
};

}
#endif
