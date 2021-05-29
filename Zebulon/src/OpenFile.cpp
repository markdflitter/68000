#include "OpenFile.h"
#include "FAT.h"
#include <string.h>
#include <bsp.h>
#include <stdio.h>

using namespace std;

OpenFile::OpenFile (FileHeader::Ptr fileHeader)
	: m_fileHeader (fileHeader),
	  m_bufferPointer (0),
	  m_bufferModified (false),
	  m_fileHeaderModified (false)
{
	setFilePointer (0);
}

OpenFile::~OpenFile ()
{
	flush ();
	if (m_fileHeaderModified) 
	{
		printf ("saving FAT\n\r");
		m_fileHeader->fat ()->save ();
	}
}

void OpenFile::read (unsigned char* data, file_address_t numBytes)	
{
	//printf ("read %d bytes from %d\n\r", numBytes, m_filePointer);
	unsigned char* p = data;
	while (numBytes > 0)
	{
		if (!readCurBlock ()) return ;

		file_address_t bytesToCopy = numBytes;
	
		file_address_t bytesLeftInCurrentBuffer = ide_block_size - (m_bufferPointer - m_buffer);
		if (bytesToCopy > bytesLeftInCurrentBuffer) bytesToCopy = bytesLeftInCurrentBuffer;

		p = copyFromBuffer (p, bytesToCopy);
		numBytes -= bytesToCopy;
	}
}
	
void OpenFile::write (unsigned char* data, file_address_t numBytes)	
{
	//printf ("writing %d bytes to %d\n\r", numBytes, m_filePointer);
	unsigned char* p = data;
	while (numBytes > 0)
	{
		if (!readCurBlock ()) return ;

		file_address_t bytesToCopy = numBytes;
	
		file_address_t bytesLeftInCurrentBuffer = ide_block_size - (m_bufferPointer - m_buffer);
		if (bytesToCopy > bytesLeftInCurrentBuffer) bytesToCopy = bytesLeftInCurrentBuffer;

		p = copyToBuffer (p, bytesToCopy);
		if (m_filePointer > m_fileHeader->size ())
		{
			//printf ("setting size to %d\n\r", m_filePointer);	
   			m_fileHeader->setSize (m_filePointer);
			m_fileHeaderModified = true;
		}
		numBytes -= bytesToCopy;
	}
}

bool OpenFile::EOF () const
{
	return m_filePointer >= m_fileHeader->size ();
}

unsigned char* OpenFile::copyFromBuffer (unsigned char* data, file_address_t bytesToCopy)
{
	//printf ("copying %d bytes from buffer\n\r", bytesToCopy);

	memcpy (data, m_bufferPointer, bytesToCopy);
	data += bytesToCopy;
	m_bufferPointer += bytesToCopy;
	setFilePointer (m_filePointer + bytesToCopy);
	
	return data;
}

unsigned char* OpenFile::copyToBuffer (unsigned char* data, file_address_t bytesToCopy)
{
	//printf ("copying %d bytes to buffer\n\r", bytesToCopy);

	m_bufferModified = true;

	memcpy (m_bufferPointer, data, bytesToCopy);
	data += bytesToCopy;
	m_bufferPointer += bytesToCopy;
	setFilePointer (m_filePointer + bytesToCopy);
	
	return data;
}

void OpenFile::flush ()
{
	//printf ("flush\n\r");
	writeCurBlock ();
}

void OpenFile::writeCurBlock ()
{
	if (m_bufferModified && (m_curBlock > 0))
	{
		//printf ("buffer modified - writing block %d\n\r", m_curBlock);	
		__ide_write (m_curBlock, m_buffer);
		m_bufferModified = false;
	}
}

bool OpenFile::readCurBlock ()
{
	if (!m_bufferLoaded)
	{
		//printf ("%d %d\n\r", m_fileHeader->size (), m_fileHeader->allocSize ());
		if (m_filePointer >= m_fileHeader->allocSize ())
		{
			//printf ("extending file\n\r");
			if (!m_fileHeader->fat ()->extend (m_fileHeader, 1))
				return false;
		
			m_bufferPointer = 0;
			setFilePointer (m_filePointer);
		}
		else
		{	
			//printf ("buffer not loaded - reading block %d\n\r", m_curBlock);	
			__ide_read (m_curBlock, m_buffer);
		}
		m_bufferLoaded = true;
	}

	return true;
}

block_address_t OpenFile::findBlock (file_address_t filePointer) const
{
	block_address_t result = 0;

	//printf ("find block for file pointer %d\n\r", filePointer);
	unsigned long blockIndex = (filePointer / ide_block_size) + 1;
	//printf ("block Index is %d\n\r", blockIndex);

	list<Chunk::Ptr>::const_iterator i = m_fileHeader->chunks ().begin ();

	while (i != m_fileHeader->chunks ().end ())
	{
		//printf ("check block from %d to %d\n\r", (*i)->start, (*i)->start + (*i)->length - 1);

		if ((*i)->length >= blockIndex)
		{
			//printf ("match!\n\r");
			result = (*i)->start + blockIndex -1;
			break;
		}
		else
		{
			//printf ("next\n\r");
			blockIndex -= (*i)->length;
			//printf ("blockIndex is now %d\n\r", blockIndex);
			i++;
		}
	}
	
	if (i != m_fileHeader->chunks ().end ())
	{
		//printf ("found block %d in chunk %d -> %d\n\r", result, (*i)->start, (*i)->start + (*i)->length - 1);
	}
	else
	{
		//printf ("[ERROR] FAT - not enough space\n\r");
	}

	return result;
}

void OpenFile::setFilePointer (file_address_t filePointer)
{
	//printf ("setting file pointer to %d, was %d\n\r", filePointer, m_filePointer);

	block_address_t newBlock = findBlock (filePointer);
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

		m_bufferLoaded = false;
		
		m_bufferPointer = m_buffer + (filePointer % ide_block_size);
		//printf ("set buffer pointer to %d\n\r", m_bufferPointer - m_buffer);
	}

	m_curBlock = newBlock;
	m_filePointer = filePointer;
}
