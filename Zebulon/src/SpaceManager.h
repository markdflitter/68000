#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include "block_address.h"
#include <list>
#include "Chunk.h"

class SpaceManager
{
public:
	void format (block_address_t size);

	std::list <Chunk::Ptr> allocate (block_address_t size);

	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);
private:
	std::list<Chunk::Ptr> m_free;
};

#endif
