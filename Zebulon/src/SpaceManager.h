#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include "block_address_t.h"
#include <list>
#include "Chunk.h"
#include <stdio.h>

class SpaceManager
{
public:
	void format (block_address_t size);

	std::list <Chunk::Ptr> allocate (block_address_t size, bool contiguous = false);
	void deallocate (std::list<Chunk::Ptr>& chunks);

	void serialise (unsigned char*& p) const;
	void deserialise (const unsigned char*& p);
private:
	std::list<Chunk::Ptr> m_free;
};

#endif
