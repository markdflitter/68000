#include "../private_include/FileEntry.h"
#include <stdio.h>

using namespace std;

namespace Zebulon
{

FileEntry::FileEntry (const string& name, const list<Chunk::Ptr> chunks)
	: m_name (name), m_size (0), m_chunks (chunks)
{
}

string FileEntry::name () const
{
	return m_name;
}

void FileEntry::setName (const string& name)
{
	m_name = name;
}

unsigned long FileEntry::size () const
{
	return m_size;
}

void FileEntry::setSize (unsigned long size)
{
	m_size = size;
}

unsigned long FileEntry::allocSize () const
{
	unsigned long result = 0;

	for (list<Chunk::Ptr>::const_iterator i = m_chunks.begin (); i != m_chunks.end (); i++)
		result = result + (*i)->length * 512;
	
	return result;
}

const list <Chunk::Ptr>& FileEntry::chunks () const
{
	return m_chunks;
}

list <Chunk::Ptr>& FileEntry::chunks ()
{
	return m_chunks;
}

void FileEntry::setChunks (const list <Chunk::Ptr>& chunks)
{
	m_chunks = chunks;
}

void FileEntry::extend (std::list<Chunk::Ptr>& newAllocation)
{
	m_chunks.splice (m_chunks.end (), newAllocation);
	consolidate ();

}

void FileEntry::diag () const
{
	for (std::list<Chunk::Ptr>::const_iterator i = m_chunks.begin (); i != m_chunks.end (); i++)
		printf ("  %d -> %d (length %d)\n\r", (*i)->start, (*i)->start + (*i)->length - 1, (*i)->length);
}

void FileEntry::consolidate ()
{
	std::list<Chunk::Ptr>::iterator prev = m_chunks.begin ();
	std::list<Chunk::Ptr>::iterator i = prev; i++;

	while (i != m_chunks.end ())
	{	 
		if (((*prev)->start + (*prev)->length) == (*i)->start)
		{
			(*prev)->length += (*i)->length;
			i = m_chunks.erase (i);
		}
		else
		  prev = i++;
	}
}

}
