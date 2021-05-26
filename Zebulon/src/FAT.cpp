#include "FAT.h"
#include <bsp.h>
#include <stdio.h>
#include "Serialise.h"
#include <string.h>

using namespace mdf;
using namespace std;

unsigned int FAT::m_lastIndex = 0;

FAT::FAT ()
{
	load ();
}

FAT::~FAT()
{
	m_openFiles.clear ();
	m_fileHeaders.clear ();
}

void FAT::format (block_address_t size)
{
	m_fileHeaders.clear ();
	m_spaceManager.format (size);
	m_bootTable.clear ();
	save ();
}

bool FAT::create (const string& name, block_address_t initialSize, bool contiguous)
{
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

void FAT::deleteFile (const string& name)
{
	list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin ();

	for ( ; i != m_fileHeaders.end (); i++)
		if ((*i)->name () == name)
		{
			if (!(*i)->bootable ())
			{
				m_spaceManager.deallocate ((*i)->chunks ());
				m_fileHeaders.erase (i);
				save ();
			}
			else
			{
				printf (">>  cannot delete bootable file, use unboot\n\r");
			}

	 		return ;
		}

	printf (">> file not found\n\r");
}

FILE FAT::open (const string& name)
{
	FileHeader::Ptr f = findFile (name);
	if (f.isNull ())
	{
		printf (">> file not found\n\r");
		return file_not_found;
	}
	
	m_openFiles.push_back (make_shared(new OpenFile(f)));
	
	return m_openFiles.size () - 1;
}

void FAT::close (FILE file)
{
	getOpenFile (file);
	m_openFiles [file].reset ();
}

void FAT::read (FILE file, unsigned char* data, file_address_t numBytes) const
{
	OpenFile::Ptr of = getOpenFile (file);
	if (!of.isNull ())
		of->read (data, numBytes);
}

void FAT::write (FILE file, unsigned char* data, file_address_t numBytes)
{
	OpenFile::Ptr of = getOpenFile (file);
	if (!of.isNull ())
		of->write (data, numBytes);
}

bool FAT::EOF (FILE file) const
{
	OpenFile::Ptr of = getOpenFile (file);
	if (!of.isNull ())
		return of->EOF ();

	return true;
}

list<string> FAT::ls () const
{
	list<string> result;

	for (list<FileHeader::Ptr>::const_iterator i = m_fileHeaders.begin (); i != m_fileHeaders.end (); i++)
	{
		result.push_back ((*i)->name ());
	}

	return result;
}

FileStat FAT::stat (const string& name) const
{
	FileHeader::ConstPtr file = findFile (name);
	if (!file.isNull ())
	{
		FileStat stat (file->name (), file->index (), file->bootable (), file->size (), file->allocSize ());

		for (list<Chunk::Ptr>::const_iterator j = file->chunks ().begin (); j != file->chunks ().end (); j++)
			stat.chunks.push_back (Chunk ((*j)->start, (*j)->length));
		return stat;
	}
	else
		return FileStat ();
}

void FAT::boot (const std::string& name, unsigned int index)
{
	// create boot table record: going to need to more parameters here
	// make the file bootable
	save ();
	
}

void FAT::unboot (unsigned int index)
{
	if ((index < m_bootTable.size ()) && !m_bootTable[index].isNull ())
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

FileHeader::Ptr FAT::findFile (const string& name)
{
	for (list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin (); i != m_fileHeaders.end (); i++)
		if ((*i)->name () == name)
			return (*i);

	return FileHeader::Ptr ();
}

FileHeader::ConstPtr FAT::findFile (const string& name) const
{
	for (list<FileHeader::Ptr>::const_iterator i = m_fileHeaders.begin (); i != m_fileHeaders.end (); i++)
		if ((*i)->name () == name)
			return (*i);

	return FileHeader::ConstPtr ();
}

OpenFile::ConstPtr FAT::getOpenFile (FILE file) const
{
	if ((file >= m_openFiles.size ()) || (m_openFiles [file].isNull ()))
	{
		printf (">> file not open\n\r");
		return OpenFile::ConstPtr ();
	}

	return m_openFiles [file];
}

OpenFile::Ptr FAT::getOpenFile (FILE file)
{
	if ((file >= m_openFiles.size ()) || (m_openFiles [file].isNull ()))
	{
		printf (">> file not open\n\r");
		return OpenFile::Ptr ();
	}

	return m_openFiles [file];
}


const char* FatIdent = "__Zebulon_FAT__1__";
const char* FatVersion = "1.1";

unsigned char* FAT::serialise (unsigned char* p) const
{
	p = Serialise::serialise (FatIdent, p);
	p = Serialise::serialise (FatVersion, p);
	p = Serialise::serialise ((unsigned long) m_lastIndex, p);

	p = m_spaceManager.serialise (p);

	p = Serialise::serialise (m_fileHeaders, p);	

	return p;
}

unsigned char* FAT::deserialise (unsigned char* p)
{
	m_openFiles.clear ();
	m_fileHeaders.clear ();

	string readIdent;
	p = Serialise::deserialise (readIdent, p, strlen (FatIdent));
	if (string (FatIdent) != readIdent)
	{
		printf (">>> FAT - ident mismatch.  Expected %s, got %s\n\r", FatIdent, readIdent.c_str ());
		return p;
	}

	string readVersion;
	p = Serialise::deserialise (readVersion, p, strlen (FatVersion));
	if (string (FatVersion) != readVersion)
	{
		printf (">>> FAT - version mismatch.  Expected %s, got %s\n\r", FatVersion, readVersion);
		return p;
	}

	unsigned long lastIndex;
	p = Serialise::deserialise (lastIndex, p);
	m_lastIndex = lastIndex;
	
	p = m_spaceManager.deserialise (p);
		
	p = Serialise::deserialise (m_fileHeaders, p);
	for (list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin(); i != m_fileHeaders.end (); i++)
		(*i)->setFat (this);

	printf ("> found %d files\n\r", m_fileHeaders.size ());
	
	return p;
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
	
	p = serialise (p);
	
	printf ("saved FAT: size %d\n\r", p - block);
	if ((p - block) > 512)
		printf (">>> FAT Block is full!!!!\n\r");

	__ide_write (0, block);	
}

void FAT::load ()
{
	unsigned char block [512];
	unsigned char* p = block;
	
	__ide_read (0, block);	

	p = deserialise (p);
}
