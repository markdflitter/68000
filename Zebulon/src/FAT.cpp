#include "FAT.h"

FAT::FAT ()
{
}

FAT::FAT (size_t size) : m_spaceManager (size)
{
}

FAT::File FAT::createFile (const std::string& name, size_t size)
{
	File result;
	result.m_name = name;
	result.m_chunks = m_spaceManager.allocate (size);
	
	m_files.push_front (result);
		
	save ();

	return result;
}

std::list<FAT::File> FAT::ls () const
{
	return m_files;
}

void FAT::load ()
{
}

void FAT::save ()
{
}
