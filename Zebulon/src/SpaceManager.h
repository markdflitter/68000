#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include <list>
#include "Chunk.h"
#include <ZebulonAPI.h>

namespace Zebulon
{

class SpaceManager
{
public:
	int initialise (int initialSize);

	std::list <Chunk::Ptr> allocate (unsigned long size, bool contiguous = false);
	void deallocate (std::list<Chunk::Ptr>& chunks);

	void serialise (unsigned char*& p) const;
	void deserialise (const unsigned char*& p);

	void diag () const;
	FreeSpace getFreeSpace () const;
private:	
	static void printFreeSpace (FreeSpace fs);

	unsigned long m_totalSpace;
	std::list<Chunk::Ptr> m_free;
};

}
#endif