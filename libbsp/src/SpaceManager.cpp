#include "../include/SpaceManager.h"

SpaceManager::SpaceManager ()
 	: m_free (0)
{
	load ();
}

SpaceManager::SpaceManager (size_t size)
 : m_free (0)
{
	format (size);
}

SpaceManager::~SpaceManager ()
{
	if (m_free)
	{
		delete m_free;
		m_free = 0;
	}
}


SpaceManager::Chunk* SpaceManager::allocate (size_t size)
{
	Chunk* allocation = 0;
	
	if (m_free)
	{
		if (m_free->m_length >= size)
		{
			allocation = new Chunk ();
			allocation->m_start = m_free->m_start;
			allocation->m_length = size;
			m_free->m_start += size;
			m_free->m_length -= size;
		
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
	m_free->m_start = 2;
	m_free->m_length = size;

	save ();
}

void SpaceManager::save () const
{
}

void SpaceManager::load ()
{
}
