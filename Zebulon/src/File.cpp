#include "File.h"

using namespace std;

File::File (FAT* fat, const string& name, const list<Chunk::Ptr> chunks)
	: m_fat (fat), m_name (name), m_chunks (chunks), m_size (0)
{
}

const FAT* File::fat () const
{
	return m_fat;
}


void File::setFat (const FAT* fat)
{
	m_fat = fat;
}

string File::name () const
{
	return m_name;
}

void File::setName (const string& name)
{
	m_name = name;
}

file_address_t File::size () const
{
	return m_size;
}

void File::setSize (file_address_t size)
{
	m_size = size;
}

const list <Chunk::Ptr>& File::chunks () const
{
	return m_chunks;
}

void File::setChunks (const list <Chunk::Ptr>& chunks)
{
	m_chunks = chunks;
}

file_address_t File::allocSize () const
{
	file_address_t result = 0;

	for (list<Chunk::Ptr>::const_iterator i = m_chunks.begin (); i != m_chunks.end (); i++)
		result = (*i)->length * 512;
	
	return result;
}
