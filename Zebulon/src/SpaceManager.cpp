#include "SpaceManager.h"
#include "Serialise.h"
#include <stdio.h>

void SpaceManager::format (block_address_t size)
{
	m_free.clear ();
	m_free.push_back (Chunk (1, size));
}

std::list<Chunk> SpaceManager::allocate (block_address_t size)
{
	std::list<Chunk> allocation;

	if (m_free.size () > 0)
	{
		Chunk& first = *(m_free.begin ());
		if (first.m_length >= size)
		{
			allocation.push_back (Chunk (first.m_start, size));
			first.m_start += size;
			first.m_length -= size;
		}
	}

	return allocation;
}	

unsigned char* SpaceManager::serialise (unsigned char* p) const
{
	return Serialise::serialise (m_free, p);
}

unsigned char* SpaceManager::deserialise (unsigned char* p)
{
	p = Serialise::deserialise (m_free, p);

 	printf ("loaded %d free chunks\n\r",m_free.size ());

	return p;
}
