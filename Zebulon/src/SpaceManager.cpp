#include "SpaceManager.h"
#include "Serialise.h"
#include <stdio.h>

using namespace mdf;

void SpaceManager::format (block_address_t size)
{
	m_free.clear ();
	m_free.push_back (make_shared (new Chunk (1, size)));
}

std::list<Chunk::Ptr> SpaceManager::allocate (block_address_t size)
{
	std::list<Chunk::Ptr> allocation;

	if (m_free.size () > 0)
	{
		Chunk::Ptr first = *(m_free.begin ());
		if (first->length >= size)
		{
			allocation.push_back (make_shared (new Chunk (first->start, size)));
			first->start += size;
			first->length -= size;
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
	m_free.clear ();
	p = Serialise::deserialise (m_free, p);

 	printf (">> loaded %d free chunks\n\r",m_free.size ());

	return p;
}
