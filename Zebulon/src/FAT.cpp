#include "FAT.h"
#include <string>
#include <string.h>
#include <bsp.h>
#include <stdio.h>
#include "Serialise.h"


FAT::OpenFile::OpenFile (const File& file)
	: m_file (file), m_filePointer (0), m_bufferPointer (m_buffer)
{
}

FAT::OpenFile::~OpenFile ()
{
}

void FAT::OpenFile::write (unsigned char* data, size_t numBytes)	
{
	printf ("writing %d bytes to %d\n\r", numBytes, m_filePointer);

	unsigned char* p = data;
	while (numBytes > 0)
	{
		size_t bytesLeftInCurrentBuffer = 512 - (m_bufferPointer - m_buffer);
		size_t bytesToCopy = bytesLeftInCurrentBuffer;
		if (numBytes < bytesToCopy) bytesToCopy = numBytes;
	
		printf ("copying %d bytes to buffer\n\r", bytesToCopy);

		memcpy (m_bufferPointer, p, bytesToCopy);
		p += bytesToCopy;
		m_bufferPointer += bytesToCopy;
		numBytes -= bytesToCopy;
		m_filePointer += bytesToCopy;

		if (512 - (m_bufferPointer - m_buffer) == 0)
		{
			printf ("buffer full, flushing\n\r");
			if (!flush ()) return;
		}
	}
	
}

bool FAT::OpenFile::flush ()
{
	size_t blockNum = (m_filePointer / 512);
	printf ("blockNum %d\n\r", blockNum);

	std::list<SpaceManager::Chunk>::const_iterator i = m_file.chunks ().begin ();
	unsigned long block; 

	while ((blockNum > 0) && (i != m_file.chunks ().end ()))
	{
		if ((*i).m_length >= blockNum)
		{
			block = (*i).m_start + blockNum;
			blockNum = 0;
		}
		else
		{
			blockNum -= (*i).m_length;
			i++;
		}
	}
	
	if (i != m_file.chunks ().end ())
	{
		printf ("found block %d in chunk %d -> %d\n\r", block, (*i).m_start, (*i).m_start + (*i).m_length);
		//__ide_write (m_buffer, (*i).m_start		

		m_bufferPointer = m_buffer;
		return true;
	}
	else
	{
		printf ("[ERROR] FAT - not enough space\n\r");
		return false;
	}
}


FAT::File::File ()
{
}

FAT::File::File (const std::string& name)
	: m_name (name)
{
}

std::string& FAT::File::name ()
{
	return m_name;
}

std::list <SpaceManager::Chunk>& FAT::File::chunks ()
{
	return m_chunks;
}

const std::string& FAT::File::name () const
{
	return m_name;
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

FAT::File FAT::createFile (const std::string& name, size_t size)
{
	File result;
	result.name () = name;
	result.chunks () = m_spaceManager.allocate (size);
	
	m_files.push_back (result);
		
	save ();

	return result;
}

std::list<FAT::File> FAT::ls () const
{
	return m_files;
}

void FAT::format (size_t size)
{
	m_spaceManager.format (size);
	m_files.clear ();
	save ();
}

const char* FatIdent = "__Zebulon_FAT__";
const unsigned int version = 5;

unsigned char* FAT::serialise (unsigned char* p) const
{
	p = Serialise::serialise (FatIdent, p);
	p = Serialise::serialise (version, p);

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

	unsigned int readVersion = 0;
	p = Serialise::deserialise (readVersion, p);
	if (readVersion != version)
	{
		printf ("[ERROR] FAT - version mismatch.  Expected %d, got %d\n\r", version, readVersion);
		return p;
	}

	p = m_spaceManager.deserialise (p);
	
	p = Serialise::deserialise (m_files, p);

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
