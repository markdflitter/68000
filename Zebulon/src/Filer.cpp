#include "../private_include/Filer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <bsp.h>
#include <algorithm>
#include "../private_include/Utils.h"
#include "../private_include/version.h"
#include <ZebulonAPI.h>
#include "../private_include/Constants.h"

using namespace mdf;
using namespace std;

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

Filer::Filer () : m_initialised (false)
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

void Filer::save ()
{
	do_save ();
}

int Filer::format (int diskSize)
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return 0;
	}

	m_openFiles.clear ();
	m_fileSearches.clear ();
  	m_bootTable.clear ();
 	int result = m_FAT.initialise (diskSize);

	if (!m_FAT.createFile (".BootTable", 1, true))
		printf (">>> failed to create boot table\n\r");

 	if (!m_FAT.createFile (".FAT", 1, true))
		printf (">>> failed to create FAT\n\r");

	return result;
}

bool Filer::createFile (const std::string& name, unsigned long initialSize, bool contiguous)
{
	return m_FAT.createFile (name, initialSize, contiguous);
}

bool Filer::deleteFile (const std::string& name)
{
	bool removedBootTableEntry = false; 

	FileEntry::Ptr f = m_FAT.findFile (name);
	if (f.isNull ())
	{
		printf (">> file not found\n\r");
		return false;
	}
	
	std::list <Chunk::Ptr>& chunks = f->chunks ();
	for (std::list <Chunk::Ptr>::iterator i = chunks.begin (); i != chunks.end (); i++)
	{
		unsigned int startBlock = (*i)->start;
		removedBootTableEntry = m_bootTable.removeEntry (startBlock);
		break;
	}

	bool deletedFile = m_FAT.deleteFile (name);
	return removedBootTableEntry || deletedFile;
}

bool Filer::rightsizeFile (FileEntry::Ptr fileEntry, unsigned long totalBlocks)
{
	return m_FAT.rightsizeFile (fileEntry, totalBlocks);
}

int Filer::fopen (const std::string& name, const std::string& mode)
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return file_not_found;
	}

	FileEntry::Ptr f = m_FAT.findFile (name);
	if (f.isNull ())
	{
		// printf ("file doesn't exist\n\r");
		if (mode_is (mode, 'w'))
		{
			//printf ("create file\n\r");
	    	m_FAT.createFile (name);
		}
		else
		{
			//printf ("file not found\n\r");
			return file_not_found;
		}			
	}
	else
	{
		//printf ("file exists\n\r");
		if (mode_is (mode, 'w'))
		{
			//printf ("truncate file\n\r");
			f->setSize (0);
		}
	}

	f = m_FAT.findFile (name);
	if (f.isNull ())
	{
		printf (">> file not found\n\r");
		return file_not_found;
	}

	file_handle index = findFirstFreeHandle (m_openFiles);
	OpenFile::Ptr file = make_shared (new OpenFile(f, guard ()));
	if (index == -1)
	{
		m_openFiles.push_back (file);
		index = m_openFiles.size () - 1;	
	}
	else
		m_openFiles [index] = file;
	
	if (mode_is (mode, 'r'))
		set_read_only (index);
	
	return index;
}

void Filer::fclose (file_handle handle)
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return ;
	}

	check_read_only (handle);
	if (!getOpenFile (handle).isNull ())
		m_openFiles [handle].reset ();	
}

bool Filer::feof (file_handle handle)
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return true;
	}

	check_read_only (handle);
	OpenFile::Ptr of = getOpenFile (handle);
	if (!of.isNull ())
		return of->EOF ();

	return true;
}

unsigned long Filer::fwrite (file_handle handle, const unsigned char* data, unsigned long numBytes)
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return 0;
	}

	if (check_read_only (handle))
	{
		printf (">> file opened as read-only\n\r");
		return 0;
	}

	OpenFile::Ptr of = getOpenFile (handle);
	if (!of.isNull ())
		return of->write (data, numBytes);

	return 0;
}

unsigned long Filer::fread (file_handle handle, unsigned char* data, unsigned long numBytes)
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return 0;
	}

	check_read_only (handle);
	OpenFile::Ptr of = getOpenFile (handle);
	if (!of.isNull ())
		return of->read (data, numBytes);

	return 0;
}

int Filer::statFile (const std::string& name, _zebulon_stats* stats)	
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return -1;
	}

	return m_FAT.statFile (name, stats);
}

Filer::file_search_handle Filer::findFirstFile (char filename [FILENAME_BUFFER_SIZE])
{
	int index = -1;
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return index;
	}

	FileSearch::Ptr fileSearch = m_FAT.findFirstFile ();
	FileEntry::Ptr fileEntry = fileSearch->next ();

	if (!fileEntry.isNull ())
	{
		memcpy (filename, fileEntry->name ().c_str (), fileEntry->name ().length ());
		filename [fileEntry->name ().length ()] = '\0';

		index = findFirstFreeHandle (m_fileSearches);
		if (index == -1)
		{
			m_fileSearches.push_back (fileSearch);
			index = m_fileSearches.size () - 1;	
		}
		else
			m_fileSearches [index] = fileSearch;
	}

	return index;
}

bool Filer::findNextFile (file_search_handle handle, char filename [FILENAME_BUFFER_SIZE])
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return false;
	}

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
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return ;
	}

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
		break;
	}

	return result;
}

void Filer::index (_zebulon_boot_table_entry btes[9])
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		memset (btes, 0, sizeof (_zebulon_boot_table_entry) * 9);
		return ;
	}

	m_bootTable.index (btes);
}

const unsigned int FATIdent = 0xDEEFACED;
const unsigned int ThisFATIdent = FATIdent + (atol (MAJOR) << 8) + atol (MINOR);

void Filer::diag () const
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return ;
	}

	m_FAT.diag ();
	printf ("FAT block ident is 0x%x\n\r\n\r", ThisFATIdent);
	m_bootTable.diag ();
}

_zebulon_free_space Filer::getFreeSpace () const
{
	if (!m_initialised)
	{
		printf (">>> filer not initialised\n\r");
		return _zebulon_free_space();
	}

	return m_FAT.getFreeSpace ();
}

template <class T>
int Filer::findFirstFreeHandle (const T& vec) const
{
    for (int index = 0; index < vec.size (); index++)
		if (vec[index].isNull ()) return index;

	return -1;
}

void Filer::do_load ()
{
	m_initialised = do_load_bootTable () && do_load_FAT ();
}

bool Filer::do_load_bootTable ()
{
	unsigned char ide_block [ide_block_size];
	::ide_result result = __ide_read (0, ide_block, _zebulon_time, _zebulon_time() + diskTimeoutInMS);	
	if (result != ::IDE_OK) 
	{
		Utils::printIdeError (result);
		return false;
	}
	
	const unsigned char* p = ide_block;
	m_bootTable.deserialise (p);
	
	return true;
}

bool Filer::do_load_FAT ()
{	
	bool result = true;
	
	unsigned char ide_block [ide_block_size];
	unsigned int blockNum = 1;

	unsigned char* buffer = 0;
	unsigned char* p = 0;
	unsigned int bytesLeft = 0;

	do
	{
		::ide_result ide_result = __ide_read (blockNum, ide_block, _zebulon_time, _zebulon_time () + diskTimeoutInMS);	
		if (ide_result != ::IDE_OK) 
		{
			Utils::printIdeError (ide_result);
			result = false;
			break ;
		}

		unsigned int FATIdent = 0;
		memcpy (&FATIdent, ide_block, sizeof (ThisFATIdent));
		//printf ("FATIdent = 0x%x\n\r", FATIdent); 
		if (FATIdent != ThisFATIdent)
		{	
			printf (">>> bad FAT ident, expected 0x%x, got 0x%x\n\r", FATIdent, ThisFATIdent);
			break ;
		}

		unsigned int FATSize = 0;
		memcpy (&FATSize, ide_block + sizeof (ThisFATIdent), sizeof (FATSize));
		//printf ("FATSize = %d\n\r", FATSize); 

		memcpy (&blockNum, ide_block + sizeof (ThisFATIdent) + sizeof (FATSize), sizeof (blockNum));
		//printf ("next block num = %d\n\r", blockNum); 

		// special case for first block, set things up
		if (buffer == 0)
		{
			buffer = new unsigned char [FATSize];
			p = buffer;
			bytesLeft = FATSize;
		}

		unsigned int bytesThisTime = min (bytesLeft, ide_block_size - sizeof (FATSize) - sizeof (blockNum) - sizeof (ThisFATIdent));
		memcpy (p, ide_block + sizeof (FATSize) + sizeof (blockNum) + sizeof (ThisFATIdent), bytesThisTime);
		p += bytesThisTime;
		bytesLeft -= bytesThisTime;	
	} while (blockNum != 0);

	if (blockNum == 0)
	{
		const unsigned char* p = buffer;
		m_FAT.deserialise (p);
	}

	delete buffer;
	return result;
}

void Filer::do_save ()
{
	do_save_bootTable ();
	do_save_FAT ();
}

void Filer::do_save_bootTable ()
{
	FileEntry::Ptr bootTable = m_FAT.findFile (".BootTable");
	if (bootTable.isNull ())
	{
		printf (">>> missing boot table\n\r");
		return ;
	}

	// the boot table is fixed size, one block is sufficient
	unsigned char ide_block [ide_block_size];
	unsigned char* p = ide_block;

	m_bootTable.serialise (p, false);

	bootTable->setSize (p - ide_block);

	const list<Chunk::Ptr>& chunks = bootTable->chunks ();
	list<Chunk::Ptr>::const_iterator i = chunks.begin ();
	unsigned int blockNum = (*i)->start;

	::ide_result result = __ide_write (blockNum, ide_block, _zebulon_time, _zebulon_time () + diskTimeoutInMS);
	if (result != ::IDE_OK)
		Utils::printIdeError (result);
}

namespace
{

unsigned int numBlocks (unsigned int numBytes)
{
	unsigned int result = numBytes / ide_block_size;
	if (numBytes % ide_block_size != 0) result++;

	return result;
}

}

void Filer::do_save_FAT ()
{
	// work out the size in blocks
	unsigned char* p = 0;
	m_FAT.serialise (p, true);
	unsigned int FATSize = (unsigned int) p;
	//printf ("FATSize %d\n\r", FATsize);

	unsigned int FATBlocks = numBlocks (FATSize);
	unsigned int sizeOnDisk = FATSize + (FATBlocks * (sizeof (unsigned int) + sizeof (unsigned int) + sizeof (unsigned int)));
	FATBlocks = numBlocks (sizeOnDisk);
	//printf ("FATBlocks = %d\n\r", FATBlocks);	

	FileEntry::Ptr FAT = m_FAT.findFile (".FAT");
	if (FAT.isNull ())
	{
		printf (">>> missing FAT\n\r");
		return ;
	}

	m_FAT.rightsizeFile (FAT, FATBlocks);
	FAT->setSize (sizeOnDisk);

	unsigned char* buffer = new unsigned char [FATSize];
	p = buffer;
	m_FAT.serialise (p, false);
	//printf ("serialised %d\n\r", p - buffer);

	const list<Chunk::Ptr>& chunks = FAT->chunks ();
	list<Chunk::Ptr>::const_iterator i = chunks.begin ();
	unsigned int blockNum = (*i)->start;

	p = buffer;
	while (p < buffer + FATSize)
	{
		unsigned char ide_block [ide_block_size];

		// write the ident, to all blocks for consistency
		memcpy (ide_block, (const unsigned char*) &ThisFATIdent, sizeof (ThisFATIdent));

		// write the size, to all blocks for consistency
		memcpy (ide_block + sizeof (ThisFATIdent), (const unsigned char*) &FATSize, sizeof (FATSize));

		// write the link
		unsigned int nextBlockNum = blockNum;
		if (nextBlockNum < (*i)->start + (*i)->length - 1)
			nextBlockNum++;
		else
		{
			if (i != chunks.end ()) i++;
			if (i == chunks.end ())
				nextBlockNum = 0;
			else			
				nextBlockNum = (*i)->start;
		}
		memcpy (ide_block + sizeof (ThisFATIdent) + sizeof (FATSize), (const unsigned char*) &nextBlockNum, sizeof (nextBlockNum));
	
		// write the data
		unsigned int bytesThisTime = min (ide_block_size - sizeof (FATSize) - sizeof (nextBlockNum) - sizeof (ThisFATIdent), buffer + FATSize - p);
		//printf ("%d\n\r", bytesThisTime);			
		memcpy (ide_block + sizeof (ThisFATIdent) + sizeof (FATSize) + sizeof (nextBlockNum), p, bytesThisTime);
		p += bytesThisTime;

		::ide_result result = __ide_write (blockNum, ide_block, _zebulon_time, _zebulon_time () + diskTimeoutInMS);
		if (result != ::IDE_OK)
			Utils::printIdeError (result);

		blockNum = nextBlockNum;
	}

	delete buffer;
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
