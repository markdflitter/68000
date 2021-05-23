#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include "block_address.h"
#include "Chunk.h"
#include <list>

class SpaceManager
{
public:
	void format (block_address_t size);

	std::list <Chunk> allocate (block_address_t size);

	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);
private:
	std::list<Chunk> m_free;
};

#endif
