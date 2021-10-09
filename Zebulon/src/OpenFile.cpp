#include "OpenFile.h"
#include "FAT.h"
#include <string.h>
#include <bsp.h>
#include <stdio.h>

using namespace std;

namespace Zebulon
{

OpenFile::OpenFile (FileEntry::Ptr fileEntry, FAT* fat)
	: m_fileEntry (fileEntry),
	  m_FAT (fat),
	  m_bufferPointer (0),
	  m_bufferModified (false),
	  m_fileEntryModified (false)
{
	setFilePointer (0);
}

OpenFile::~OpenFile ()
{
	flush ();
	if (m_fileEntryModified) 
	{
		m_FAT->save ();
	}
}

void OpenFile::read (unsigned char* data, unsigned long numBytes)	
{
	//printf ("read %d bytes from %d\n\r", numBytes, m_filePointer);
	unsigned char* p = data;
	while (numBytes > 0)
	{
		if (!readCurBlock ()) return ;

		unsigned long bytesToCopy = numBytes;
	
		unsigned long bytesLeftInCurrentBuffer = ide_block_size - (m_bufferPointer - m_buffer);
		if (bytesToCopy > bytesLeftInCurrentBuffer) bytesToCopy = bytesLeftInCurrentBuffer;

		p = copyFromBuffer (p, bytesToCopy);
		numBytes -= bytesToCopy;
	}
}
	
void OpenFile::write (const unsigned char* data, unsigned long numBytes)	
{
	//printf ("writing %d bytes to %d\n\r", numBytes, m_filePointer);
	const unsigned char* p = data;
	while (numBytes > 0)
	{
		if (!readCurBlock ()) return ;

		unsigned long bytesToCopy = numBytes;
	
		unsigned long bytesLeftInCurrentBuffer = ide_block_size - (m_bufferPointer - m_buffer);
		if (bytesToCopy > bytesLeftInCurrentBuffer) bytesToCopy = bytesLeftInCurrentBuffer;

		p = copyToBuffer (p, bytesToCopy);
		if (m_filePointer > m_fileEntry->size ())
		{
			//printf ("setting size to %d\n\r", m_filePointer);	
   			m_fileEntry->setSize (m_filePointer);
			m_fileEntryModified = true;
		}
		numBytes -= bytesToCopy;
	}
}

bool OpenFile::EOF () const
{
	return m_filePointer >= m_fileEntry->size ();
}

unsigned char* OpenFile::copyFromBuffer (unsigned char* data, unsigned long bytesToCopy)
{
	//printf ("copying %d bytes from buffer\n\r", bytesToCopy);

	memcpy (data, m_bufferPointer, bytesToCopy);
	data += bytesToCopy;
	m_bufferPointer += bytesToCopy;
	setFilePointer (m_filePointer + bytesToCopy);
	
	return data;
}

const unsigned char* OpenFile::copyToBuffer (const unsigned char* data, unsigned long bytesToCopy)
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
		//printf ("%d %d\n\r", m_fileEntry->size (), m_fileEntry->allocSize ());
		if (m_filePointer >= m_fileEntry->allocSize ())
		{
			//printf ("extending file\n\r");
			if (!m_FAT->extendFile (m_fileEntry, 1))
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

unsigned long OpenFile::findBlock (unsigned long filePointer) const
{
	unsigned long result = 0;

	//printf ("find block for file pointer %d\n\r", filePointer);
	unsigned long blockIndex = (filePointer / ide_block_size) + 1;
	//printf ("block Index is %d\n\r", blockIndex);

	list<Chunk::Ptr>::const_iterator i = m_fileEntry->chunks ().begin ();

	while (i != m_fileEntry->chunks ().end ())
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
	
	if (i != m_fileEntry->chunks ().end ())
	{
		//printf ("found block %d in chunk %d -> %d\n\r", result, (*i)->start, (*i)->start + (*i)->length - 1);
	}
	else
	{
		//printf ("[ERROR] FAT - not enough space\n\r");
	}

	return result;
}

void OpenFile::setFilePointer (unsigned long filePointer)
{
	//printf ("setting file pointer to %d, was %d\n\r", filePointer, m_filePointer);

	unsigned long newBlock = findBlock (filePointer);
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
		
		m_bufferPointer = m_buffer + (filePointer % 512);
		//printf ("set buffer pointer to %d\n\r", m_bufferPointer - m_buffer);
	}

	m_curBlock = newBlock;
	m_filePointer = filePointer;
}

}
