#include "SpaceManager.h"
#include <shared_ptr>
#include "Serialise.h"
#include <stdio.h>

using namespace mdf;

namespace Zebulon
{

int SpaceManager::initialise (int initialSize)
{
	m_free.clear ();
	m_free.push_back (make_shared (new Chunk (0, initialSize)));

	return initialSize;
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

}

