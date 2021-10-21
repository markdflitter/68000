#include "../private_include/FAT.h"
#include "../private_include/Serialise.h"
#include <string.h>
#include <string>
#include <ZebulonAPI.h>
#include "../private_include/version.h"
#include "../private_include/Utils.h"

using namespace std;
using namespace mdf;

namespace Zebulon
{

int FAT::initialise (int diskSize)
{
	int result = m_spaceManager.initialise (diskSize);
	m_fileEntries.clear ();
	return result;
}

void FAT::serialise (unsigned char*& p, bool sizeOnly) const
{
	Serialise::serialise (FAT_IDENT, p, sizeOnly);
	Serialise::serialise (FAT_VERSION, p, sizeOnly);
	//Serialise::serialise ((unsigned long) m_lastIndex, p, sizeOnly);

	m_spaceManager.serialise (p, sizeOnly);

	Serialise::serialise (m_fileEntries, p, sizeOnly);	
}

bool FAT::deserialise (const unsigned char*& p)
{
	m_fileEntries.clear ();

	string readIdent;
	Serialise::deserialise (readIdent, p, strlen (FAT_IDENT));
	if (string (FAT_IDENT) != readIdent)
	{
		printf (">>> FAT - ident mismatch.  Expected %s, got %s\n\r", FAT_IDENT, readIdent.c_str ());
		return false;
	}

	string readVersion;
	Serialise::deserialise (readVersion, p, strlen (FAT_VERSION));
	if (string (FAT_VERSION) != readVersion)
	{
		printf (">>> FAT - version mismatch.  Expected %s, got %s\n\r", FAT_VERSION, readVersion.c_str());
		return false;
	}

	//unsigned long lastIndex;
	//Serialise::deserialise (lastIndex, p);
	//m_lastIndex = lastIndex;
	
	m_spaceManager.deserialise (p);
		
	Serialise::deserialise (m_fileEntries, p);

	printf (" found %d file(s)\n\r", m_fileEntries.size ());
	
	return true;
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
	if (name.length () > MAX_FILENAME_LENGTH)
	{
		printf (">> filename may not be > %d characters\n\r", MAX_FILENAME_LENGTH);
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
	 		return true;
		}

	return false;
}

bool FAT::extendFile (FileEntry::Ptr fileEntry, unsigned long numBlocks)
{
	unsigned int allocSize = fileEntry->allocSize ();
	unsigned int allocBlocks = allocSize / ide_block_size;
	if (allocSize % ide_block_size != 0)
		allocBlocks++;

	if (numBlocks > allocBlocks)
	{
		unsigned int newBlocks = numBlocks - allocBlocks;
		list<Chunk::Ptr> newAllocation = m_spaceManager.allocate (newBlocks);

		if ((numBlocks > 0) && (newAllocation.size () == 0))
		{
			printf (">>> disk full\n\r");
			return false;
		}
	
		fileEntry->extend (newAllocation);
	}

	return true;
}

int FAT::statFile (const std::string& name, _zebulon_stats* stats)
{
	FileEntry::Ptr file = findFile (name);
	if (!file.isNull ())
	{
		stats->size = file->size ();
		stats->sizeOnDisk = file->allocSize ();
		return 0;
	}

	printf (">> file not found\n\r");
	return -1;
}

FileSearch::Ptr FAT::findFirstFile ()
{
	return make_shared(new FileSearch (m_fileEntries));
}

void FAT::diag () const
{
	m_spaceManager.diag ();

	printf ("--- files ---\n\r");
	int n = 0;
	for (list<FileEntry::Ptr>::const_iterator i = m_fileEntries.begin (); i != m_fileEntries.end (); i++)
	{
		printf ("%d\t: %s %d byte(s)\n\r", n, Utils::padFilename ((*i)->name ()).c_str(), (*i)->size ());
		(*i)->diag ();
		n++;
	}

	printf ("\n\r");	
	unsigned char* p = 0;
	serialise (p, true);
	printf ("FAT size %d byte(s)\n\r", (unsigned int) p);

	printf ("\n\r");	
}

_zebulon_free_space FAT::getFreeSpace () const
{
	return m_spaceManager.getFreeSpace ();
}

}

