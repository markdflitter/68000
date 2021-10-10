#include "SpaceManager.h"
#include <shared_ptr>
#include "Serialise.h"
#include <stdio.h>

using namespace mdf;


namespace
{

void printFreeSpace (Zebulon::_zebulon_free_space fs)
{
	printf ("total free: %d out of %d (%d%%)\n\r", fs.freeSpace, fs.totalSpace, ((unsigned int) (100 * double(fs.freeSpace) / fs.totalSpace)));
}

}


namespace Zebulon
{

int SpaceManager::initialise (int initialSize)
{
	m_totalSpace = initialSize;

	m_free.clear ();
	m_free.push_back (make_shared (new Chunk (2, initialSize - 2)));

	return initialSize;
}

std::list<Chunk::Ptr> SpaceManager::allocate (unsigned long size, bool contiguous)
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
	Serialise::serialise (m_totalSpace, p);
	Serialise::serialise (m_free, p);
}

void SpaceManager::deserialise (const unsigned char*& p)
{
	Serialise::deserialise (m_totalSpace, p);

	m_free.clear ();
	Serialise::deserialise (m_free, p);

 	printf ("loaded %d free chunk(s)\n\r",m_free.size ());
	printFreeSpace (getFreeSpace ());
}

void SpaceManager::diag () const
{
	printf ("--- Space Table ---\n\r");
	int n  = 0;
	unsigned int free = 0;
	for (std::list<Chunk::Ptr>::const_iterator i = m_free.begin (); i != m_free.end (); i++)
	{
		printf (" %d : free chunk from %d -> %d (length %d)\n\r", n, (*i)->start, (*i)->start + (*i)->length, (*i)->length);
		n++;	
		free += (*i)->length;
	}

	printFreeSpace (getFreeSpace ());
	printf ("\n\r");
}


_zebulon_free_space SpaceManager::getFreeSpace () const
{
	_zebulon_free_space fs;
	fs.freeSpace = 0;
	fs.totalSpace = m_totalSpace;

	for (std::list<Chunk::Ptr>::const_iterator i = m_free.begin (); i != m_free.end (); i++)
		fs.freeSpace += (*i)->length;

	return fs;	
}

}

