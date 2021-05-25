#include "FileHeader.h"

using namespace std;

FileHeader::FileHeader (FAT* fat, const string& name, const list<Chunk::Ptr> chunks)
	: m_fat (fat), m_name (name), m_chunks (chunks), m_size (0)
{
}

FAT* FileHeader::fat () 
{
	return m_fat;
}


void FileHeader::setFat (FAT* fat)
{
	m_fat = fat;
}

string FileHeader::name () const
{
	return m_name;
}

void FileHeader::setName (const string& name)
{
	m_name = name;
}

file_address_t FileHeader::size () const
{
	return m_size;
}

void FileHeader::setSize (file_address_t size)
{
	m_size = size;
}

const list <Chunk::Ptr>& FileHeader::chunks () const
{
	return m_chunks;
}

list <Chunk::Ptr>& FileHeader::chunks ()
{
	return m_chunks;
}

void FileHeader::setChunks (const list <Chunk::Ptr>& chunks)
{
	m_chunks = chunks;
}

file_address_t FileHeader::allocSize () const
{
	file_address_t result = 0;

	for (list<Chunk::Ptr>::const_iterator i = m_chunks.begin (); i != m_chunks.end (); i++)
		result = result + (*i)->length * 512;
	
	return result;
}

void FileHeader::extend (std::list<Chunk::Ptr>& newAllocation)
{
	m_chunks.splice (m_chunks.end (), newAllocation);
}
