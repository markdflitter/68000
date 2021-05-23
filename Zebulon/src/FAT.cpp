#include "FAT.h"
#include <bsp.h>
#include <stdio.h>
#include "Serialise.h"
#include <string.h>

FAT::FAT ()
{
	load ();
}

File FAT::createFile (const std::string& name, block_address_t size)
{
	File result (this, name, m_spaceManager.allocate (size));
	m_files.push_back (result);
		
	save ();

	return result;
}

std::list<File>& FAT::ls ()
{
	return m_files;
}

void FAT::format (block_address_t size)
{
	m_spaceManager.format (size);
	m_files.clear ();
	save ();
}

const char* FatIdent = "__Zebulon_FAT__1__";
const char* FatVersion = "1.0";

unsigned char* FAT::serialise (unsigned char* p) const
{
	p = Serialise::serialise (FatIdent, p);
	p = Serialise::serialise (FatVersion, p);

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

	std::string readVersion;
	p = Serialise::deserialise (readVersion, p, strlen (FatVersion));
	if (std::string (FatVersion) != readVersion)
	{
		printf ("[ERROR] FAT - version mismatch.  Expected %s, got %s\n\r", FatVersion, readVersion);
		return p;
	}

	p = m_spaceManager.deserialise (p);
	
	p = Serialise::deserialise (m_files, p);
	for (std::list<File>::iterator i = m_files.begin(); i != m_files.end (); i++)
		(*i).setFat (this);

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
