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
const char* FatVersion = "2.2";

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

bool FAT::createFile (const std::string& name, unsigned long initialSize, bool contiguous)
{
	if (name.length () > 64)
	{
		printf (">> filename may not be > 64 characters\n\r");
		return false;
	}

	if (!findFile (name).isNull ())
	{
		printf (">> file already exists\n\r");
		return false;
	}

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

bool FAT::deleteFile (const std::string& name)
{
	list<FileEntry::Ptr>::iterator i = m_fileEntries.begin ();

	for ( ; i != m_fileEntries.end (); i++)
		if ((*i)->name () == name)
		{
			m_spaceManager.deallocate ((*i)->chunks ());
			m_fileEntries.erase (i);
			save ();
	 		return true;
		}

	printf (">> file not found\n\r");
	return false;
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

_zebulon_stats FAT::statFile (const std::string& name)
{
	_zebulon_stats stats;
	stats.size = 0;
	stats.sizeOnDisk = 0;

	FileEntry::Ptr file = findFile (name);
	if (!file.isNull ())
	{
		stats.size = file->size ();
		stats.sizeOnDisk = file->allocSize ();
	}

	return stats;
}


void FAT::serialise (unsigned char*& p) const
{
	Serialise::serialise (FatIdent, p);
	Serialise::serialise (FatVersion, p);
	//Serialise::serialise ((unsigned long) m_lastIndex, p);

	m_spaceManager.serialise (p);

	Serialise::serialise (m_fileEntries, p);	
}

bool FAT::deserialise (const unsigned char*& p)
{
	m_fileEntries.clear ();

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
		
	Serialise::deserialise (m_fileEntries, p);

	printf ("found %d file(s)\n\r", m_fileEntries.size ());
	
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

void FAT::diag () const
{
	m_spaceManager.diag ();

	printf ("--- files ---\n\r");
	int n = 0;
	for (list<FileEntry::Ptr>::const_iterator i = m_fileEntries.begin (); i != m_fileEntries.end (); i++)
	{
		string name = (*i)->name ();
		
		string pad (64 - name.length (), ' ');	

		printf ("%d\t: %s%s %d byte(s)\n\r", n, (*i)->name ().c_str(), pad.c_str (), (*i)->size ());
		n++;
	}

	printf ("\n\r");	
}

_zebulon_free_space FAT::getFreeSpace () const
{
	return m_spaceManager.getFreeSpace ();
}

}

