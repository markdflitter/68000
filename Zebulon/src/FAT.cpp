#include "FAT.h"
#include "File.h"
#include "OpenFile.h"
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
	m_files.clear ();
}

void FAT::format (block_address_t size)
{
	m_files.clear ();
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
	
	m_files.push_back (make_shared (new File (this, name, m_spaceManager.allocate (initialSize))));
	save ();	
}

FILE FAT::open (const string& name)
{
	File::Ptr f = findFile (name);
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
	if ((file >= m_openFiles.size ()) || (m_openFiles [file].isNull ()))
	{
		printf ("not open\n\r");
		return;
	}

	m_openFiles [file].reset ();
}

list<File::Ptr>& FAT::ls ()
{
	return m_files;
}

File::Ptr FAT::findFile (const string& name)
{
	for (list<File::Ptr>::iterator i = m_files.begin (); i != m_files.end (); i++)
		if ((*i)->name () == name)
			return (*i);

	return File::Ptr ();
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
	m_openFiles.clear ();
	m_files.clear ();

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
		
	p = Serialise::deserialise (m_files, p);
	for (list<File::Ptr>::iterator i = m_files.begin(); i != m_files.end (); i++)
		(*i)->setFat (this);

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
