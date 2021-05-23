#include "FAT.h"
#include <string>
#include <string.h>
#include <bsp.h>
#include <stdio.h>
#include "Serialise.h"


FAT::OpenFile::OpenFile (File& file)
	: m_file (file), m_bufferPointer (0), m_bufferModified (false)
{
	setFilePointer (0);
}

FAT::OpenFile::~OpenFile ()
{
	flush ();
	m_file.fat->save ();
}

void FAT::OpenFile::read (unsigned char* data, file_address_t numBytes)	
{
	//printf ("read %d bytes from %d\n\r", numBytes, m_filePointer);
	unsigned char* p = data;
	while (numBytes > 0)
	{
		readCurBlock ();

		file_address_t bytesToCopy = numBytes;
	
		file_address_t bytesLeftInCurrentBuffer = 512 - (m_bufferPointer - m_buffer);
		if (bytesToCopy > bytesLeftInCurrentBuffer) bytesToCopy = bytesLeftInCurrentBuffer;

		p = copyFromBuffer (p, bytesToCopy);
		numBytes -= bytesToCopy;
	}
}
	
void FAT::OpenFile::write (unsigned char* data, file_address_t numBytes)	
{
	//printf ("writing %d bytes to %d\n\r", numBytes, m_filePointer);
	unsigned char* p = data;
	while (numBytes > 0)
	{
		readCurBlock ();

		file_address_t bytesToCopy = numBytes;
	
		file_address_t bytesLeftInCurrentBuffer = 512 - (m_bufferPointer - m_buffer);
		if (bytesToCopy > bytesLeftInCurrentBuffer) bytesToCopy = bytesLeftInCurrentBuffer;

		p = copyToBuffer (p, bytesToCopy);
		if (m_filePointer > m_file.bytes ())
			m_file.bytes () = m_filePointer;

		numBytes -= bytesToCopy;
	}
}

bool FAT::OpenFile::EOF () const
{
	return m_filePointer >= m_file.bytes ();
}

unsigned char* FAT::OpenFile::copyFromBuffer (unsigned char* data, file_address_t bytesToCopy)
{
	//printf ("copying %d bytes from buffer\n\r", bytesToCopy);

	memcpy (data, m_bufferPointer, bytesToCopy);
	data += bytesToCopy;
	m_bufferPointer += bytesToCopy;
	setFilePointer (m_filePointer + bytesToCopy);
	
	return data;
}


unsigned char* FAT::OpenFile::copyToBuffer (unsigned char* data, file_address_t bytesToCopy)
{
	//printf ("copying %d bytes to buffer\n\r", bytesToCopy);

	m_bufferModified = true;

	memcpy (m_bufferPointer, data, bytesToCopy);
	data += bytesToCopy;
	m_bufferPointer += bytesToCopy;
	setFilePointer (m_filePointer + bytesToCopy);
	
	return data;
}


void FAT::OpenFile::flush ()
{
	//printf ("flush\n\r");
	writeCurBlock ();
}

void FAT::OpenFile::writeCurBlock ()
{
	if (m_bufferModified)
	{
		//printf ("buffer modified - writing block %d\n\r", m_curBlock);	
		__ide_write (m_curBlock, m_buffer);
		m_bufferModified = false;
	}
}

void FAT::OpenFile::readCurBlock ()
{
	if (!m_bufferLoaded)
	{	
		//printf ("buffer not loaded - reading block %d\n\r", m_curBlock);	
		__ide_read (m_curBlock, m_buffer);
		m_bufferLoaded = true;
	}
}

bool FAT::OpenFile::findBlock (file_address_t filePointer, SpaceManager::block_address_t& block)
{
	//printf ("find block for file pointer %d\n\r", filePointer);
	unsigned long blockIndex = (filePointer / 512) + 1;
	//printf ("block Index is %d\n\r", blockIndex);

	std::list<SpaceManager::Chunk>::iterator i = m_file.chunks ().begin ();

	while (i != m_file.chunks ().end ())
	{
		//printf ("check block from %d to %d\n\r", (*i).m_start, (*i).m_start + (*i).m_length - 1);

		if ((*i).m_length >= blockIndex)
		{
			//printf ("match!\n\r");
			block = (*i).m_start + blockIndex -1;
			break;
		}
		else
		{
			//printf ("next\n\r");
			blockIndex -= (*i).m_length;
			//printf ("blockIndex is now %d\n\r", blockIndex);
			i++;
		}
	}
	
	if (i != m_file.chunks ().end ())
	{
		//printf ("found block %d in chunk %d -> %d\n\r", block, (*i).m_start, (*i).m_start + (*i).m_length - 1);
		return true;
	}
	else
	{
		//printf ("[ERROR] FAT - not enough space\n\r");
		return false;
	}
}


void FAT::OpenFile::setFilePointer (file_address_t filePointer)
{
	//printf ("setting file pointer to %d, was %d\n\r", filePointer, m_filePointer);

	SpaceManager::block_address_t newBlock;
	bool validBlock = findBlock (filePointer, newBlock);

	//printf ("new block is %d, was %d\n\r", newBlock, m_curBlock);

	if (m_bufferPointer == 0 || (newBlock != m_curBlock))
	{
		if (m_bufferPointer == 0)
		{
			//printf ("first read\n\r");
			;
		}
		else
		{
			//printf ("block changed\n\r");
			flush ();		
		}

		m_bufferLoaded = !validBlock;
		
		m_bufferPointer = m_buffer + (filePointer % 512);
		//printf ("set buffer pointer to %d\n\r", m_bufferPointer - m_buffer);
	}

	m_curBlock = newBlock;
	m_filePointer = filePointer;
}

FAT::File::File ()
	: m_bytes (0)
{
}

FAT::File::File (const std::string& name)
	: m_name (name), m_bytes (0)
{
}

std::string FAT::File::name () const
{
	return m_name;
}

void FAT::File::setName (const std::string& name)
{
	m_name = name;
}

FAT::file_address_t& FAT::File::bytes ()
{
	return m_bytes;
}

std::list <SpaceManager::Chunk>& FAT::File::chunks ()
{
	return m_chunks;
}

FAT::file_address_t FAT::File::allocSize () const
{
	FAT::file_address_t result = 0;

	for (std::list<SpaceManager::Chunk>::const_iterator i = m_chunks.begin (); i != m_chunks.end (); i++)
		result = (*i).m_length * 512;
	
	return result;
}
	
const FAT::file_address_t& FAT::File::bytes () const
{
	return m_bytes;
}

const std::list <SpaceManager::Chunk>& FAT::File::chunks () const
{
	return m_chunks;
}

FAT::OpenFile FAT::File::open ()
{
	return OpenFile (*this);
}


FAT::FAT ()
{
	load ();
}

FAT::File FAT::createFile (const std::string& name, SpaceManager::block_address_t size)
{
	File result;
	result.setName (name);
	result.chunks () = m_spaceManager.allocate (size);
	result.fat = this;
	
	m_files.push_back (result);
		
	save ();

	return result;
}

std::list<FAT::File>& FAT::ls ()
{
	return m_files;
}

void FAT::format (SpaceManager::block_address_t size)
{
	m_spaceManager.format (size);
	m_files.clear ();
	save ();
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
	std::string readIdent;
	p = Serialise::deserialise (readIdent, p, strlen (FatIdent));
	if (std::string (FatIdent) != readIdent)
	{
		printf ("[ERROR] FAT - ident mismatch.  Expected %s, got %s\n\r", FatIdent, readIdent.c_str ());
		return p;
	}

	std::string readVersion;
	p = Serialise::deserialise (readVersion, p, strlen (FatVersion));
	if (std::string (FatVersion) != readVersion)
	{
		printf ("[ERROR] FAT - version mismatch.  Expected %s, got %s\n\r", FatVersion, readVersion);
		return p;
	}

	p = m_spaceManager.deserialise (p);
	
	p = Serialise::deserialise (m_files, p);
	for (std::list<File>::iterator i = m_files.begin(); i != m_files.end (); i++)
		(*i).fat = this;

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
