#include "SpaceManager.h"

SpaceManager::SpaceManager ()
{
	load ();
}

SpaceManager::SpaceManager (size_t size)
{
	format (size);
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
		
			save ();
		}
	}

	return allocation;
}	

void SpaceManager::print () const
{
}

void SpaceManager::format (size_t size)
{
	m_free.push_back (Chunk (2, size));
	save ();
}

void SpaceManager::save () const
{
}

void SpaceManager::load ()
{
}
