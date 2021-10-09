#include "FAT.h"
#include "Utils.h"
#include "Serialise.h"
#include <bsp.h>
#include <string.h>
#include <string>

using namespace std;
using namespace mdf;

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
	if (name.length () > 255)
	{
		printf (">> filename may not be > 255 characters\n\r");
		return false;
	}

	if (!findFile (name).isNull ())
	{
		printf (">> file already exists\n\r");
		return false;
	}

	printf ("create %d\n\r", initialSize);
	list<Chunk::Ptr> allocation = m_spaceManager.allocate (initialSize, contiguous);
	if ((initialSize > 0) && (allocation.size () == 0))
	{
		printf (">>> disk full\n\r");
		return false;
	}

	m_fileEntries.push_back (make_shared (new FileEntry (name, allocation)));

	save ();	
	return true;
}

void FAT::deleteFile (const std::string& name)
{
	list<FileEntry::Ptr>::iterator i = m_fileEntries.begin ();

	for ( ; i != m_fileEntries.end (); i++)
		if ((*i)->name () == name)
		{
			m_spaceManager.deallocate ((*i)->chunks ());
			m_fileEntries.erase (i);
			save ();
	 		return ;
		}

	printf (">> file not found\n\r");
}

bool FAT::extendFile (FileEntry::Ptr fileEntry, unsigned long numBlocks)
{
	list<Chunk::Ptr> newAllocation = m_spaceManager.allocate (numBlocks);

	if ((numBlocks > 0) && (newAllocation.size () == 0))
	{
		printf (">>> disk full\n\r");
		return false;
	}

	fileEntry->extend (newAllocation);
	save ();

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

FileEntry::Ptr FAT::findFile (const string& name)
{
	FileEntry::Ptr result;

	if (name.length () > 0)
	{
		for (list<FileEntry::Ptr>::iterator i = m_fileEntries.begin (); i != m_fileEntries.end (); i++)
			if ((*i)->name () == name)
			{
				result = (*i);
				break ;
			}
	}

	return result;
}

}

