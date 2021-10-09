#include "FileEntry.h"

using namespace std;

namespace Zebulon
{

FileEntry::FileEntry (const string& name, const list<Chunk::Ptr> chunks)
	: m_name (name), m_chunks (chunks)
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
}

}
