#include "FAT.h"
#include <string>
#include <string.h>
#include <bsp.h>
#include <stdio.h>

FAT::FAT ()
{
	load ();
}

FAT::File FAT::createFile (const std::string& name, size_t size)
{
	File result;
	result.m_name = name;
	result.m_chunks = m_spaceManager.allocate (size);
	
	m_files.push_front (result);
		
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

const char* FatIdent = "FAT";
const unsigned int version = 1;
	
void FAT::save () const
{
	unsigned char block [512];

	unsigned int i = 0;
	memcpy (&(block[i]), FatIdent, strlen (FatIdent) + 1); i += strlen (FatIdent) + 1;
	memcpy (&(block[i]), &version, version); i += sizeof (version);
	size_t files = m_files.size ();
	memcpy (&(block[i]), &files, sizeof (files)); i += sizeof (files);
	for (std::list<File>::const_iterator j = m_files.begin (); j != m_files.end (); j++)
	{
		memcpy (&(block[i]), &(*j), sizeof (File)); i += sizeof (File);
	}
			
	__ide_write (1, block);	
}

void FAT::load ()
{
	unsigned char block [512];
	__ide_read (1, block);	
	
	unsigned int i = 0;
	char strbuffer [strlen (FatIdent) + 1];
	memcpy (&strbuffer, &(block[i]), strlen (FatIdent) + 1); i += strlen (FatIdent) + 1;
	strbuffer [strlen (FatIdent)] = '\0';
	std::string readIdent (strbuffer);
	if (std::string (FatIdent) != std::string (readIdent))
	{
		printf ("[ERROR] FAT::ident mismatch.  Expected %s, got %s\n\r", FatIdent, readIdent);
		return ;
	}

	unsigned int readVersion = 0;
	memcpy (&readVersion, &(block[i]), sizeof (version)); i += sizeof (version);
	if (readVersion != 1)
	{
		printf ("[ERROR] FAT::version mismatch.  Expected %d, got %d\n\r", 1, readVersion);
		return ;
	}

	size_t numFiles = 0;
	memcpy (&numFiles, &(block[i]), sizeof (numFiles)); i += sizeof (numFiles);
	for (unsigned int file = 0; i < numFiles; i++)
	{
		File newFile;
		memcpy (&newFile, &(block[i]), sizeof (newFile)); i += sizeof (newFile);
		m_files.push_back (newFile);
	}
}
