#include "Filer.h"
#include <stdio.h>

using namespace mdf;

namespace
{
bool mode_is (const std::string& mode, char what)
{
	const char* str = mode.c_str ();
	char ch;
	while ((ch = *str++) != '\0')
	{
		if (ch == what) return true;
	}
	
	return false;
}

void set_read_only (int& handle)
{
	handle |= 0x8000;
}

bool check_read_only (int& handle)
{
	bool result = (handle & 0x8000) == 0x8000;
	handle &= ~0x8000;
	return result;
}

}


namespace Zebulon
{

Filer::Filer ()
{
	printf ("Filer::Filer()\n\r");
}

Filer::~Filer ()
{
	m_openFiles.clear ();
}

void Filer::load ()
{
	m_FAT.load ();
}

int Filer::format (int diskSize)
{
  	return m_FAT.initialise (diskSize);
}

int Filer::fopen (const std::string& name, const std::string& mode)
{
	file_handle result = file_not_found;

	if (m_FAT.findFile (name).isNull ())
	{
		//printf ("file doesn't exist\n\r");
		if (mode_is (mode, 'w'))
		{
			//printf ("create file\n\r");
	    	m_FAT.createFile (name);
		}
		else
		{
			//printf ("file not found\n\r");
			return result;
		}			
	}
	else
	{
		//printf ("file exists\n\r");
		if (mode_is (mode, 'w'))
		{
			//printf ("delete and create file\n\r");
			m_FAT.deleteFile (name);
	    	m_FAT.createFile (name);
		}
	}

	FileEntry::Ptr f = m_FAT.findFile (name);
	if (f.isNull ())
	{
		printf (">> file not found\n\r");
		return file_not_found;
	}
	
	m_openFiles.push_back (make_shared(new OpenFile(f, &m_FAT)));

	result = m_openFiles.size () - 1;

	if (mode_is (mode, 'r'))
		set_read_only (result);
	
	return result;
}

void Filer::fclose (file_handle handle)
{
	check_read_only (handle);
	if (!getOpenFile (handle).isNull ())
		m_openFiles [handle].reset ();	
}

bool Filer::feof (file_handle handle)
{
	check_read_only (handle);
	OpenFile::Ptr of = getOpenFile (handle);
	if (!of.isNull ())
		return of->EOF ();

	return true;
}

unsigned long Filer::fwrite (file_handle handle, const unsigned char* data, unsigned long numBytes)
{
	if (check_read_only (handle))
	{
		printf (">> file opened as read-only\n\t");
		return 0;
	}

	OpenFile::Ptr of = getOpenFile (handle);
	if (!of.isNull ())
		return of->write (data, numBytes);

	return 0;
}

unsigned long Filer::fread (file_handle handle, unsigned char* data, unsigned long numBytes)
{
	check_read_only (handle);
	OpenFile::Ptr of = getOpenFile (handle);
	if (!of.isNull ())
		return of->read (data, numBytes);

	return 0;
}

_zebulon_stats Filer::statFile (const std::string& name)
{
	return m_FAT.statFile (name);
}

bool Filer::deleteFile (const std::string& name)
{
	return m_FAT.deleteFile (name);
}

void Filer::diag () const
{
	m_FAT.diag ();
}

FreeSpace Filer::getFreeSpace () const
{
	return m_FAT.getFreeSpace ();
}

OpenFile::Ptr Filer::getOpenFile (file_handle file)
{
	if ((file >= m_openFiles.size ()) || (m_openFiles [file].isNull ()))
	{
		printf (">> file %d not open\n\r", file);
		return OpenFile::Ptr ();
	}

	return m_openFiles [file];
}


}
