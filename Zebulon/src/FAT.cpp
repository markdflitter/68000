#include "FAT.h"
#include "Utils.h"
#include "Serialise.h"
#include <bsp.h>
#include <string.h>
#include <string>

using namespace std;

namespace Zebulon
{

const char* FatIdent = "__Zebulon_FAT__1__";
const char* FatVersion = "2.0";

int FAT::initialise (int diskSize)
{
	int result = m_spaceManager.initialise (diskSize);
	do_save ();
	return result;
}

void FAT::load ()
{
	do_load ();
}

void FAT::save ()
{
	do_save ();
}

bool FAT::createFile (const std::string& name, unsigned long initialSize, bool contiguous)
{
	return true;
}

void FAT::deleteFile (const std::string& name)
{
}


bool FAT::extendFile (FileEntry::Ptr fileEntry, unsigned long numBlocks)
{
	return true;
}

void FAT::serialise (unsigned char*& p) const
{
	Serialise::serialise (FatIdent, p);
	Serialise::serialise (FatVersion, p);
	//Serialise::serialise ((unsigned long) m_lastIndex, p);

	m_spaceManager.serialise (p);

	//Serialise::serialise (m_fileHeaders, p);	
}

bool FAT::deserialise (const unsigned char*& p)
{
	//m_openFiles.clear ();
	//m_fileHeaders.clear ();

	string readIdent;
	Serialise::deserialise (readIdent, p, strlen (FatIdent));
	if (string (FatIdent) != readIdent)
	{
		printf (">>> FAT - ident mismatch.  Expected %s, got %s\n\r", FatIdent, readIdent.c_str ());
		return false;
	}

	string readVersion;
	Serialise::deserialise (readVersion, p, strlen (FatVersion));
	if (string (FatVersion) != readVersion)
	{
		printf (">>> FAT - version mismatch.  Expected %s, got %s\n\r", FatVersion, readVersion.c_str());
		return false;
	}

	//unsigned long lastIndex;
	//Serialise::deserialise (lastIndex, p);
	//m_lastIndex = lastIndex;
	
	m_spaceManager.deserialise (p);
		
	//Serialise::deserialise (m_fileHeaders, p);
	//for (list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin(); i != m_fileHeaders.end (); i++)
	//	(*i)->setFat (this);

	//printf ("> found %d files\n\r", m_fileHeaders.size ());
	
	return true;
}


void FAT::do_load ()
{
	unsigned char block [512];
	const unsigned char* p = block;
	
	::ide_result result = __ide_read (1, block);	
	if (result != ::IDE_OK) 
	{
		Utils::printIdeError (result);
		return ;
	}

	if (!deserialise (p))
		return ;

	//p = block;
	
	//result = __ide_read (0, block);	
	//if (result != OK)
	//{
	//	printIdeError (result);
	//	return ;
	//}

	//Serialise::deserialise (m_bootTable, p);
}

void FAT::do_save () const
{
	unsigned char block [1024];
	unsigned char* p = block;

	//Serialise::serialise (m_bootTable, p);

	//ide_result result = __ide_write (0, block);
	//if (result != OK)
	//	printIdeError (result);

	//p = block;
	serialise (p);
	
	if (p - block > 400)
	{
		printf (">> FAT size is now %d bytes\n\r", p - block);
		if ((p - block) > 512)
			printf (">>> FAT block is full!!!!\n\r");
	}

	::ide_result result = __ide_write (1, block);	
	if (result != ::IDE_OK)
		Utils::printIdeError (result);
}

}

