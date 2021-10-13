#include "../private_include/Filer.h"
#include <stdio.h>
#include <string.h>
#include <bsp.h>
#include "../private_include/Utils.h"

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
	m_fileSearches.clear ();
}

void Filer::load ()
{
	do_load ();
}

void Filer::save () const
{
	do_save ();
}

int Filer::format (int diskSize)
{
	m_openFiles.clear ();
	m_fileSearches.clear ();
  	m_bootTable.clear ();
 	int result = m_FAT.initialise (diskSize);

	do_save ();

	return result;
}

bool Filer::createFile (const std::string& name, unsigned long initialSize, bool contiguous)
{
	bool result = m_FAT.createFile (name, initialSize, contiguous);
	if (result) save ();
	return result;
}

bool Filer::deleteFile (const std::string& name)
{
	bool result = m_FAT.deleteFile (name);
	if (result) save ();
	return result;
}

bool Filer::extendFile (FileEntry::Ptr fileEntry, unsigned long numBlocks)
{
	bool result = m_FAT.extendFile (fileEntry, numBlocks);
	if (result) save ();
	return result;
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
	
	m_openFiles.push_back (make_shared(new OpenFile(f, this)));

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

int Filer::statFile (const std::string& name, _zebulon_stats* stats)	
{
	return m_FAT.statFile (name, stats);
}

Filer::file_search_handle Filer::findFirstFile (char filename [FILENAME_BUFFER_SIZE])
{
	FileSearch::Ptr fileSearch = m_FAT.findFirstFile ();
	FileEntry::Ptr fileEntry = fileSearch->next ();

	if (!fileEntry.isNull ())
	{
		memcpy (filename, fileEntry->name ().c_str (), fileEntry->name ().length ());
		filename [fileEntry->name ().length ()] = '\0';

		m_fileSearches.push_back (fileSearch);
	
		return m_fileSearches.size () - 1;
	}
	else
		return -1;
}

bool Filer::findNextFile (file_search_handle handle, char filename [FILENAME_BUFFER_SIZE])
{
	FileSearch::Ptr fs = getFileSearch (handle);
	if (fs.isNull ())
	{
		printf (">> search %d not found\n\r", handle);
		return false;
	}

	FileEntry::Ptr fileEntry = fs->next ();
	if (!fileEntry.isNull ())
	{
		memcpy (filename, fileEntry->name ().c_str (), fileEntry->name ().length ());
		filename [fileEntry->name ().length ()] = '\0';

		return true;
	}
	else
		return false;
}

void Filer::closeFind (file_search_handle handle)
{
	if (!getFileSearch (handle).isNull ())
		m_fileSearches [handle].reset ();
}

bool Filer::boot (unsigned int slot, const std::string& filename, unsigned int loadAddress, unsigned int startAddress, unsigned int length)
{
	bool result = false;

	FileEntry::Ptr f = m_FAT.findFile (filename);
	if (f.isNull ())
	{
		printf (">> file not found\n\r");
		return result;
	}
	
	std::list <Chunk::Ptr>& chunks = f->chunks ();
	for (std::list <Chunk::Ptr>::iterator i = chunks.begin (); i != chunks.end (); i++)
	{
		unsigned int startBlock = (*i)->start;
		result = m_bootTable.addEntry (slot, filename, loadAddress, startAddress, length, startBlock);
		if (result) do_save ();	
		break;
	}

	return result;
}

void Filer::diag () const
{
	m_FAT.diag ();
	m_bootTable.diag ();
}

_zebulon_free_space Filer::getFreeSpace () const
{
	return m_FAT.getFreeSpace ();
}

void Filer::do_load ()
{
	unsigned char block [512];
	const unsigned char* p = block;
	
	{
		::ide_result result = __ide_read (1, block);	
		if (result != ::IDE_OK) 
		{
			Utils::printIdeError (result);
			return ;
		}
	}

	if (!m_FAT.deserialise (p))
		return ;

	p = block;
	
	{
		::ide_result result = __ide_read (0, block);	
		if (result != ::IDE_OK)
		{
			Utils::printIdeError (result);
			return ;
		}
	}

	m_bootTable.deserialise (p);
}

void Filer::do_save () const
{
	unsigned char block [1024];
	unsigned char* p = block;

	m_bootTable.serialise (p);

	{
		::ide_result result = __ide_write (0, block);
		if (result != ::IDE_OK)
			Utils::printIdeError (result);
	}

	p = block;
	m_FAT.serialise (p);
	
	if (p - block > 400)
	{
		printf (">> FAT size is now %d bytes\n\r", p - block);
		if ((p - block) > 512)
			printf (">>> FAT block is full!!!!\n\r");
	}

	{
		::ide_result result = __ide_write (1, block);	
		if (result != ::IDE_OK)
			Utils::printIdeError (result);
	}
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

FileSearch::Ptr Filer::getFileSearch (file_search_handle handle)
{
	if ((handle >= m_fileSearches.size ()) || (m_fileSearches [handle].isNull ()))
	{
		printf (">> search %d not found\n\r", handle);
		return FileSearch::Ptr ();
	}

	return m_fileSearches [handle];
}

}
