#ifndef BOOTTABLE_H
#define BOOTTABLE_H

#include "BootTableEntry.h"
#include <vector>
#include <ZebulonAPI.h>

namespace Zebulon
{

class BootTable
{
public:
	BootTable ();

	void clear ();

	bool addEntry (unsigned int slot, const std::string& filename, unsigned int loadAddress, unsigned int startAddress, unsigned int length, unsigned int startBlock);
	bool removeEntry (unsigned int startBlock);

	void index (_zebulon_boot_table_entry btes[9]) const;

	void serialise (unsigned char*& p) const;
	bool deserialise (const unsigned char*& p);

	void diag () const;
private:
  std::vector<BootTableEntry::Ptr> m_entries;
};

}

#endif


