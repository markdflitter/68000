#include "SpaceManager.h"
#include <string>
#include <string.h>
#include <bsp.h>
#include <stdio.h>
#include "Serialise.h"

SpaceManager::SpaceManager ()
{
}

std::list<SpaceManager::Chunk> SpaceManager::allocate (size_t size)
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

void SpaceManager::print () const
{
	unsigned int chunk = 0;
	for (std::list<Chunk>::const_iterator i = m_free.begin (); i != m_free.end (); i++)
	{
		printf ("%d : %d -> %d\n\r", chunk, (*i).m_start, (*i).m_start + (*i).m_length);
	}
}

void SpaceManager::format (size_t size)
{
	m_free.clear ();
	m_free.push_back (Chunk (2, size));
}

unsigned char* SpaceManager::serialise (unsigned char* p) const
{
	return Serialise::serialise (m_free, p);
}

unsigned char* SpaceManager::deserialise (unsigned char* p)
{
	p = Serialise::deserialise (m_free, p);

 	printf ("loaded %d free chunks\n\r",m_free.size ());
	print ();

	return p;
}
