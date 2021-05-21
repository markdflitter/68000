#include "SpaceManager.h"
#include <string>
#include <string.h>
#include <bsp.h>
#include <stdio.h>

SpaceManager::SpaceManager ()
{
	load ();
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
	unsigned int chunk = 0;
	for (std::list<Chunk>::const_iterator i = m_free.begin (); i != m_free.end (); i++)
	{
		printf ("%d : %d -> %d", chunk, (*i).m_start, (*i).m_start + (*i).m_length);
	}
}

void SpaceManager::format (size_t size)
{
	m_free.clear ();
	m_free.push_back (Chunk (2, size));
	save ();
}

const char* spaceManagerIdent = "SpaceManager";
const unsigned int version = 1;
	
void SpaceManager::save () const
{
	unsigned char block [512];

	unsigned int i = 0;
	memcpy (&(block[i]), spaceManagerIdent, strlen (spaceManagerIdent) + 1); i += strlen (spaceManagerIdent) + 1;
	memcpy (&(block[i]), &version, version); i += sizeof (version);
	size_t chunks = m_free.size ();
	memcpy (&(block[i]), &chunks, sizeof (chunks)); i += sizeof (chunks);
	for (std::list<Chunk>::const_iterator j = m_free.begin (); j != m_free.end (); j++)
	{
		memcpy (&(block[i]), &(*j), sizeof (Chunk)); i += sizeof (Chunk);
	}
			
	__ide_write (0, block);	
}

void SpaceManager::load ()
{
	unsigned char block [512];
	__ide_read (0, block);	
	
	unsigned int i = 0;
	char strbuffer [strlen (spaceManagerIdent) + 1];
	memcpy (&strbuffer, &(block[i]), strlen (spaceManagerIdent) + 1); i += strlen (spaceManagerIdent) + 1;
	strbuffer [strlen (spaceManagerIdent)] = '\0';
	std::string readIdent (strbuffer);
	if (std::string (spaceManagerIdent) != std::string (readIdent))
	{
		printf ("[ERROR] SpaceManager::ident mismatch.  Expected %s, got %s\n\r", spaceManagerIdent, readIdent);
		return ;
	}

	unsigned int readVersion = 0;
	memcpy (&readVersion, &(block[i]), sizeof (version)); i += sizeof (version);
	if (readVersion != 1)
	{
		printf ("[ERROR] SpaceManager::version mismatch.  Expected %d, got %d\n\r", 1, readVersion);
		return ;
	}

	size_t numChunks = 0;
	memcpy (&numChunks, &(block[i]), sizeof (numChunks)); i += sizeof (numChunks);
	for (unsigned int chunk = 0; i < numChunks; i++)
	{
		Chunk newChunk;
		memcpy (&newChunk, &(block[i]), sizeof (newChunk)); i += sizeof (newChunk);
		m_free.push_back (newChunk);
	}
}
