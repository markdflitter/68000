#ifndef BOOTTABLE_H
#define BOOTTABLE_H

#include "BootTableEntry.h"
#include <vector>


namespace Zebulon
{

class BootTable
{
public:
	void addEntry ();
	void deleteEntry ();

	void serialise () const;
	void deserialise ();
private:
  std::vector<BootTableEntry::Ptr> m_entries;
};

}

#endif


