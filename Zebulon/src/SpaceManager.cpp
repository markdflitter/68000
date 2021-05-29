#include "SpaceManager.h"
#include "Serialise.h"
#include <stdio.h>

using namespace mdf;

void SpaceManager::format (block_address_t size)
{
	m_free.clear ();
	m_free.push_back (make_shared (new Chunk (0, size)));
}

std::list<Chunk::Ptr> SpaceManager::allocate (block_address_t size, bool contiguous)
{
	std::list<Chunk::Ptr> allocation;

	if (size > 0)
	{
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
	}

	return allocation;
}	

void SpaceManager::deallocate (std::list<Chunk::Ptr>& chunks)
{
	m_free.splice (m_free.end (), chunks);
}

void SpaceManager::serialise (unsigned char*& p) const
{
	Serialise::serialise (m_free, p);
}

void SpaceManager::deserialise (const unsigned char*& p)
{
	m_free.clear ();
	Serialise::deserialise (m_free, p);

 	printf ("> loaded %d free chunks\n\r",m_free.size ());
}
