#include "../private_include/BootTable.h"
#include <stdio.h>
#include "../private_include/Serialise.h"
#include <string.h>
#include "../private_include/Utils.h"

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
	for (int i = 0; i < 9; i++)
		m_entries [i] = make_shared (new BootTableEntry ());
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
		printf (">> boot slot %d is full, replacing %s\n\r", slot, m_entries [slot]->shortName.c_str ());
	}

	printf ("creating boot table entry %d for %s\n\r",slot, filename.c_str ());
	m_entries [slot] = make_shared (
		new BootTableEntry (filename, length, loadAddress, startAddress, startBlock));

	return true;
}

bool BootTable::removeEntry (unsigned int startBlock)
{
	for (size_t index = 0; index < m_entries.size (); ++index)
	{
		BootTableEntry::Ptr p = m_entries[index];
		if (!p->empty && (p->startBlock == startBlock))
		{
			printf ("removing boot table entry %d for %s\n\r", index, p->shortName.c_str ());
			p->empty = true;
			return true;
		}
	}

	return false;
}

void BootTable::index (_zebulon_boot_table_entry btes[9]) const
{
	for (size_t i = 0; i < m_entries.size (); ++i)
	{
		BootTableEntry::Ptr p = m_entries[i];
		btes [i].empty = p->empty;
		memcpy (btes [i].name, p->shortName.c_str (), 21);
		btes [i].length = p->length;
		btes [i].loadAddress = p->loadAddress;
		btes [i].startAddress = p->startAddress;
		btes [i].startBlock = p->startBlock;
	}
}

void BootTable::serialise (unsigned char*& p, bool sizeOnly) const
{
	Serialise::serialise (m_entries, p, sizeOnly);
}

bool BootTable::deserialise (const unsigned char*& p)
{
	Serialise::deserialise (m_entries, p);
	return true;
}

void BootTable::diag () const
{
	printf ("--- boot table ---\n\r");

	for (size_t index = 0; index < m_entries.size (); ++index)
	{
		BootTableEntry::Ptr p = m_entries[index];

		printf ("%d : ", index);
		if (p->empty)
			printf ("empty\n\r");
		else
		{
			printf ("%s len = %d byte(s), load 0x%x, start = 0x%x, firstBlock = %d\n\r", Utils::pad (p->shortName, 20, ' ').c_str (), p->length, p->loadAddress, p->startAddress, p->startBlock);
		}
	}

	printf ("\n\r");
}


}

