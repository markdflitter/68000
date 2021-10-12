#include "../private_include/BootTable.h"
#include <stdio.h>
#include "../private_include/Serialise.h"

using namespace std;
using namespace mdf;

namespace Zebulon
{

BootTable::BootTable()
{
	clear ();
}


void BootTable::clear ()
{
	m_entries.clear ();
	m_entries.reserve (9);
}


bool BootTable::addEntry (unsigned int slot, const string& filename, unsigned int loadAddress, unsigned int startAddress, unsigned int length, unsigned int startBlock)
{
	if (slot > 9)
	{
		printf (">> max boot slot is %d, got %d\n\r", 9, slot);
		return false;
	}

	if (!m_entries [slot].isNull () && !m_entries [slot]->empty) 
	{
		printf (">> boot slot %d is full, unboot first\n\r", slot);
		return false;
	}

	printf ("creating boot table entry %d for %s\n\r",slot, filename.c_str ());
	m_entries [slot] = make_shared (
		new BootTableEntry (filename, length, loadAddress, startAddress, startBlock));

	return true;
}

void BootTable::serialise (unsigned char*& p) const
{
	Serialise::serialise (m_entries, p);
}

bool BootTable::deserialise (const unsigned char*& p)
{
	Serialise::deserialise (m_entries, p);
	return true;
}

}

