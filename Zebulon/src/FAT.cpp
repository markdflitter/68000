#include "FAT.h"
#include <string>
#include <string.h>
#include <bsp.h>
#include <stdio.h>
#include "Serialise.h"

FAT::FAT ()
{
	load ();
}

FAT::File FAT::createFile (const std::string& name, size_t size)
{
	File result;
	result.m_name = name;
	result.m_chunks = m_spaceManager.allocate (size);
	
	m_files.push_back (result);
		
	save ();

	return result;
}

std::list<FAT::File> FAT::ls () const
{
	return m_files;
}

void FAT::format (size_t size)
{
	m_spaceManager.format (size);
	m_files.clear ();
	save ();
}

const char* FatIdent = "__Zebulon_FAT__";
const unsigned int version = 5;

unsigned char* FAT::serialise (unsigned char* p) const
{
	p = Serialise::serialise (FatIdent, p);
	p = Serialise::serialise (version, p);

	p = m_spaceManager.serialise (p);

	p = Serialise::serialise (m_files, p);	

	return p;
}

unsigned char* FAT::deserialise (unsigned char* p)
{
	std::string readIdent;
	p = Serialise::deserialise (readIdent, p, strlen (FatIdent));
	if (std::string (FatIdent) != readIdent)
	{
		printf ("[ERROR] FAT - ident mismatch.  Expected %s, got %s\n\r", FatIdent, readIdent.c_str ());
		return p;
	}

	unsigned int readVersion = 0;
	p = Serialise::deserialise (readVersion, p);
	if (readVersion != version)
	{
		printf ("[ERROR] FAT - version mismatch.  Expected %d, got %d\n\r", version, readVersion);
		return p;
	}

	p = m_spaceManager.deserialise (p);
	
	p = Serialise::deserialise (m_files, p);

	printf ("found %d files\n\r", m_files.size ());
	
	return p;
}

	
void FAT::save () const
{
	unsigned char block [512];
	unsigned char* p = block;
	
	p = serialise (p);

	__ide_write (0, block);	
}

void FAT::load ()
{
	unsigned char block [512];
	unsigned char* p = block;
	
	__ide_read (0, block);	

	p = deserialise (p);
}
