#include "FAT.h"
#include <bsp.h>
#include <stdio.h>
#include "Serialise.h"
#include <string.h>
#include <stdlib.h>

using namespace mdf;
using namespace std;

unsigned int FAT::m_lastIndex = 0;

namespace
{
  	void printIdeError (ide_result error)
	{
		if (error == AMNF)
			printf (">>>  address mark not found\n\r");
		if (error == TK0NF)
			printf (">>>  track 0 not found\n\r");
		if (error == ABRT)
			printf (">>>  aborted command");
		if (error == MCR)
			printf (">>>  media change requested\n\r");
		if (error == IDNF)
			printf (">>>  ID not found\n\r");
		if (error == MC)
			printf (">>>  media change\n\r");
		if (error == UNC)
			printf (">>>  uncorrectable data error\n\r");
		if (error == BBK)
			printf (">>>  bad block\n\r");
	}
}

FAT::FAT ()
{
	m_bootTable.reserve (10);
	load ();
}

size_t FAT::blockSize () const
{
	return ide_block_size;
}

bool FAT::readBlock (block_address_t block, unsigned char* data)
{
	ide_result result = __ide_read (block, data);
	if (result != OK) printIdeError (result);

	return result == OK;
}

void FAT::format (block_address_t size)
{
	m_lastIndex = 0;
	m_fileHeaders.clear ();
	m_spaceManager.format (size);
	m_spaceManager.allocate (1);		// boot table
	m_spaceManager.allocate (1);		// FAT table
	m_bootTable.clear ();
	save ();
}

bool FAT::fileExists (const std::string& filename)
{
	FileHeader::Ptr header = findFile (filename);
	return !header.isNull ();
}

bool FAT::create (const string& name, block_address_t initialSize, bool contiguous)
{
	if (name [0] == '#')
	{
		printf (">> filename may not start with #\n\r");
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

	m_fileHeaders.push_back (make_shared (new FileHeader (this, name, m_lastIndex++, allocation)));

	save ();	
	return true;
}

void FAT::setMetaData (const std::string& name, unsigned int loadAddress, unsigned int goAddress)
{
	FileHeader::Ptr file = findFile (name);
	if (file.isNull ())
	{
		printf (">> file not found\n\r");
		return ;
	}

	if (file->bootable ())
	{
		printf (">> cannot set meta data for bootable file, unboot first\n\r");
		return ;
	}

	file->setLoadAddress (loadAddress);
	file->setGoAddress (goAddress);

	save ();
}

bool FAT::stat (const std::string& name, struct Zebulon::_zebulon_stat* zs)
{
	FileHeader::Ptr p = findFile (name);
	bool result = p.isNull ();
	if (result)
	{
		memcpy (zs->name, p->name ().c_str (), p->name ().length ());
		zs->size = p->size ();
	}

	return result;
}

file_search_handle FAT::findFirstFile (struct Zebulon::_zebulon_stat* s)
{
	FileSearch::Ptr fileSearch = make_shared(new FileSearch (m_fileHeaders));

	FileHeader::Ptr fileHeader = fileSearch->next ();

	if (!fileHeader.isNull ())
	{
		memcpy (s->name, fileHeader->name ().c_str (), fileHeader->name ().length ());
		s->size = fileHeader->size ();

		m_fileSearches.push_back (fileSearch);

		return m_fileSearches.size () - 1;
	}
	else
		return -1;
}

bool FAT::findNextFile (file_search_handle handle, struct Zebulon::_zebulon_stat* s)
{
	FileSearch::Ptr fs = getFileSearch (handle);
	if (fs.isNull ())
	{
		printf (">> search %d not found\n\r", handle);
		return false;
	}

	FileHeader::Ptr fileHeader = fs->next ();
	if (!fileHeader.isNull ())
	{
		memcpy (s->name, fileHeader->name ().c_str (), fileHeader->name ().length ());
		s->size = fileHeader->size ();

		return true;
	}
	else
		return false;
}

void FAT::closeFind (file_search_handle handle)
{
	getFileSearch (handle);
	m_fileSearches [handle].reset ();
}

void FAT::rm (const string& name)
{
	FileHeader::Ptr p = findFile (name);
	if (p.isNull ()) return ;
	
	string realName = p->name ();

	list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin ();

	for ( ; i != m_fileHeaders.end (); i++)
		if ((*i)->name () == realName)
		{
			if (!(*i)->bootable ())
			{
				m_spaceManager.deallocate ((*i)->chunks ());
				m_fileHeaders.erase (i);
				save ();
			}
			else
			{
				printf (">> cannot delete bootable file, unboot first\n\r");
			}

	 		return ;
		}

	printf (">> file not found\n\r");
}

file_handle FAT::open (const string& name)
{
	//printf ("opening file '%s'\n\r", name.c_str ());
	FileHeader::Ptr f = findFile (name);
	if (f.isNull ())
	{
		printf (">> file not found\n\r");
		return file_not_found;
	}
	
	m_openFiles.push_back (make_shared(new OpenFile(f)));

	return m_openFiles.size () - 1;
}

void FAT::close (file_handle file)
{
	getOpenFile (file);
	m_openFiles [file].reset ();
}

file_address_t FAT::read (file_handle file, unsigned char* data, file_address_t numBytes) const
{
	OpenFile::Ptr of = getOpenFile (file);
	if (!of.isNull ())
		of->read (data, numBytes);
	return 0;
}

file_address_t FAT::write (file_handle file, const unsigned char* data, file_address_t numBytes)
{
	OpenFile::Ptr of = getOpenFile (file);
	if (!of.isNull ())
		of->write (data, numBytes);
	return 0;
}

bool FAT::EOF (file_handle file) const
{
	OpenFile::Ptr of = getOpenFile (file);
	if (!of.isNull ())
		return of->EOF ();

	return true;
}

void FAT::boot (const std::string& name, unsigned int index)
{
	if (index > 9)
	{
		printf (">> max boot slot is %d\n\r", 9);
		return ;
	}

	if (!m_bootTable [index].isNull () && !m_bootTable [index]->empty) 
	{
		printf (">> boot slot %d is full, unboot first\n\r", index);
		return ;
	}
	
	FileHeader::Ptr fileHeader = findFile (name);
	if (fileHeader.isNull ())
	{
		printf (">> file not found\n\r");
		return ;
	}
	
	if (fileHeader->chunks ().size () == 0)
	{
		printf (">> file empty\n\r");
		return ;
	}
	
	if (fileHeader->bootable ())
	{
		printf (">> already bootable, unboot first\n\r");
		return ;
	}

	block_address_t startBlock = (*(fileHeader->chunks ().begin ()))->start;
	
	printf ("creating boot table entry\n\r");
	m_bootTable [index] = make_shared (
		new BootTableEntry (name, fileHeader->index (),
			fileHeader->size (), fileHeader->loadAddress (), fileHeader->goAddress (), startBlock));

	fileHeader->setBootable (true);	
	save ();
}

void FAT::unboot (unsigned int index)
{
	if (index > 9)
	{
		printf (">> max boot slot is %d\n\r", 9);
		return ;
	}

	if ((index < m_bootTable.size ()) && !m_bootTable[index].isNull () && !m_bootTable[index]->empty)
	{
		BootTableEntry::Ptr p = m_bootTable [index];
		unsigned int fileIndex = p->index;

		for (list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin (); i != m_fileHeaders.end (); i++)
			if ((*i)->index () == fileIndex)
			{
				printf ("marking file '%s' as unbootable\n\r", (*i)->name ().c_str ());
				(*i)->setBootable (false);
			}
		m_bootTable[index].reset ();
		save ();
	}
}

void FAT::index () const
{
	for (size_t i = 0; i < m_bootTable.size (); i++)
	{
		BootTableEntry::Ptr bte = m_bootTable [i];
		
		if (!bte.isNull () && !bte->empty)
			printf ("%d : %s : %d : 0x%x : 0x%x : 0x%x : %d\n\r", 
				i, bte->shortName.c_str (), bte->index, bte->length, bte->loadAddress, bte->goAddress, bte->startBlock);
	}
}

FileHeader::Ptr FAT::findFile (const string& name)
{
	FileHeader::Ptr result;

	if (name.length () > 0)
	{
		if (name [0] == '#')
		{
			if (name.length () > 1)
			{
				size_t index = atoi (string (name.c_str () + 1).c_str ());
				for (list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin (); i != m_fileHeaders.end (); i++)
					if ((*i)->index () == index)
					{
						result = (*i);
						break ;
					}
			}
		}
		else
		{
			for (list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin (); i != m_fileHeaders.end (); i++)
				if ((*i)->name () == name)
				{
					result = (*i);
					break ;
				}
		}
	}

	return result;
}

FileSearch::Ptr FAT::getFileSearch (file_search_handle handle)
{
	if ((handle >= m_fileSearches.size ()) || (m_fileSearches [handle].isNull ()))
	{
		printf (">> search %d not found\n\r", handle);
		return FileSearch::Ptr ();
	}

	return m_fileSearches [handle];
}


OpenFile::ConstPtr FAT::getOpenFile (file_handle file) const
{
	if ((file >= m_openFiles.size ()) || (m_openFiles [file].isNull ()))
	{
		printf (">> file %d not open\n\r", file);
		return OpenFile::ConstPtr ();
	}

	return m_openFiles [file];
}

OpenFile::Ptr FAT::getOpenFile (file_handle file)
{
	if ((file >= m_openFiles.size ()) || (m_openFiles [file].isNull ()))
	{
		printf (">> file %d not open\n\r", file);
		return OpenFile::Ptr ();
	}

	return m_openFiles [file];
}


const char* FatIdent = "__Zebulon_FAT__1__";
const char* FatVersion = "1.8";

void FAT::serialise (unsigned char*& p) const
{
	Serialise::serialise (FatIdent, p);
	Serialise::serialise (FatVersion, p);
	Serialise::serialise ((unsigned long) m_lastIndex, p);

	m_spaceManager.serialise (p);

	Serialise::serialise (m_fileHeaders, p);	
}

bool FAT::deserialise (const unsigned char*& p)
{
	m_openFiles.clear ();
	m_fileHeaders.clear ();

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

	unsigned long lastIndex;
	Serialise::deserialise (lastIndex, p);
	m_lastIndex = lastIndex;
	
	m_spaceManager.deserialise (p);
		
	Serialise::deserialise (m_fileHeaders, p);
	for (list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin(); i != m_fileHeaders.end (); i++)
		(*i)->setFat (this);

	printf ("> found %d files\n\r", m_fileHeaders.size ());
	
	return true;
}

bool FAT::extend (FileHeader::Ptr fileHeader, block_address_t numBlocks)
{
	list<Chunk::Ptr> newAllocation = m_spaceManager.allocate (numBlocks);

	if ((numBlocks > 0) && (newAllocation.size () == 0))
	{
		printf (">>> disk full\n\r");
		return false;
	}

	fileHeader->extend (newAllocation);
	save ();
	return true;
}
	
void FAT::save () const
{
	unsigned char block [1024];
	unsigned char* p = block;

	Serialise::serialise (m_bootTable, p);

	ide_result result = __ide_write (0, block);
	if (result != OK)
		printIdeError (result);

	p = block;
	serialise (p);
	
	if (p - block > 400)
	{
		printf (">> FAT size is now %d bytes\n\r", p - block);
		if ((p - block) > ide_block_size)
			printf (">>> FAT block is full!!!!\n\r");
	}

	result = __ide_write (1, block);	
	if (result != OK)
		printIdeError (result);
}

void FAT::load ()
{
	unsigned char block [ide_block_size];
	const unsigned char* p = block;
	
	ide_result result = __ide_read (1, block);	
	if (result != OK) 
	{
		printIdeError (result);
		return ;
	}

	if (!deserialise (p))
		return ;

	p = block;
	
	result = __ide_read (0, block);	
	if (result != OK)
	{
		printIdeError (result);
		return ;
	}

	Serialise::deserialise (m_bootTable, p);
}
