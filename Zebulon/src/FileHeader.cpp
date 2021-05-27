#include "FileHeader.h"

using namespace std;

FileHeader::FileHeader (FAT* fat, const string& name, unsigned int index, const list<Chunk::Ptr> chunks)
	: m_fat (fat), m_name (name), m_index (index), m_bootable (false), m_chunks (chunks), m_size (0), m_loadAddress (0), m_goAddress (0)
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

unsigned int FileHeader::index () const
{
	return m_index;
}

void FileHeader::setIndex (unsigned int index)
{
	m_index = index;
}

bool FileHeader::bootable () const
{
	return m_bootable;
}

void FileHeader::setBootable (bool bootable)
{
	m_bootable = bootable;
}


file_address_t FileHeader::size () const
{
	return m_size;
}

void FileHeader::setSize (file_address_t size)
{
	m_size = size;
}

unsigned int FileHeader::loadAddress () const
{
	return m_loadAddress;
}

void FileHeader::setLoadAddress (unsigned int loadAddress)
{
	m_loadAddress = loadAddress;
}

unsigned int FileHeader::goAddress () const
{
	return m_goAddress;
}

void FileHeader::setGoAddress (unsigned int goAddress)
{
	m_goAddress = goAddress;
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
