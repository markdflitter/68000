#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include <size_t.h>
#include <list>

class SpaceManager
{
public:
	typedef unsigned long block_address_t;

	SpaceManager ();	

	struct Chunk {
		Chunk () : m_start (0), m_length (0) {} 
		Chunk (block_address_t start, block_address_t length) : m_start (start), m_length (length) {} 
		block_address_t m_start;
		block_address_t m_length;
	};

	std::list <Chunk> allocate (block_address_t size);

	void print () const;

	void format (block_address_t size);

	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);
private:
	std::list<Chunk> m_free;
};

#endif
