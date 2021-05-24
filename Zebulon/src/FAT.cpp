#include "FAT.h"
#include <bsp.h>
#include <stdio.h>
#include "Serialise.h"
#include <string.h>

using namespace mdf;
using namespace std;

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
	save ();
}

void FAT::create (const string& name, block_address_t initialSize)
{
	if (!findFile (name).isNull ())
	{
		printf ("file already exists\n\r");
		return ;
	}
	
	m_fileHeaders.push_back (make_shared (new FileHeader (this, name, m_spaceManager.allocate (initialSize))));
	save ();	
}

FILE FAT::open (const string& name)
{
	FileHeader::Ptr f = findFile (name);
	if (f.isNull ())
	{
		printf ("file not found\n\r");
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
		FileStat stat (file->name (), file->size (), file->allocSize ());

		for (list<Chunk::Ptr>::const_iterator j = file->chunks ().begin (); j != file->chunks ().end (); j++)
			stat.chunks.push_back (Chunk ((*j)->start, (*j)->length));
		return stat;
	}
	else
		return FileStat ();
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
		printf ("not open\n\r");
		return OpenFile::ConstPtr ();
	}

	return m_openFiles [file];
}

OpenFile::Ptr FAT::getOpenFile (FILE file)
{
	if ((file >= m_openFiles.size ()) || (m_openFiles [file].isNull ()))
	{
		printf ("not open\n\r");
		return OpenFile::Ptr ();
	}

	return m_openFiles [file];
}


const char* FatIdent = "__Zebulon_FAT__1__";
const char* FatVersion = "1.0";

unsigned char* FAT::serialise (unsigned char* p) const
{
	p = Serialise::serialise (FatIdent, p);
	p = Serialise::serialise (FatVersion, p);

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
		printf ("[ERROR] FAT - ident mismatch.  Expected %s, got %s\n\r", FatIdent, readIdent.c_str ());
		return p;
	}

	string readVersion;
	p = Serialise::deserialise (readVersion, p, strlen (FatVersion));
	if (string (FatVersion) != readVersion)
	{
		printf ("[ERROR] FAT - version mismatch.  Expected %s, got %s\n\r", FatVersion, readVersion);
		return p;
	}

	p = m_spaceManager.deserialise (p);
		
	p = Serialise::deserialise (m_fileHeaders, p);
	for (list<FileHeader::Ptr>::iterator i = m_fileHeaders.begin(); i != m_fileHeaders.end (); i++)
		(*i)->setFat (this);

	printf (">> found %d files\n\r", m_fileHeaders.size ());
	
	return p;
}
void FAT::extend (FileHeader::Ptr fileHeader, block_address_t numBlocks)
{
	list<Chunk::Ptr> newAllocation = m_spaceManager.allocate (numBlocks);
	fileHeader->extend (newAllocation);
	save ();
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
