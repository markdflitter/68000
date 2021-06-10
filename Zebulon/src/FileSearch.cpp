#include "FileSearch.h"
#include <stdio.h>

FileSearch::FileSearch (const std::list<FileHeader::Ptr>& snapshot) :
  m_snapshot (snapshot), m_it (m_snapshot.begin ())
{
}

	
FileHeader::Ptr FileSearch::next ()
{
	FileHeader::Ptr result;
	if (m_it != m_snapshot.end ())
	{
		result = (*m_it);
		m_it++;
	}

	return result;
}

