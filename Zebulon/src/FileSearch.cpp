#include "FileSearch.h"

namespace Zebulon
{

FileSearch::FileSearch (const std::list<FileEntry::Ptr>& snapshot) :
  m_snapshot (snapshot), m_it (m_snapshot.begin ())
{
}

	
FileEntry::Ptr FileSearch::next ()
{
	FileEntry::Ptr result;
	if (m_it != m_snapshot.end ())
	{
		result = (*m_it);
		m_it++;
	}

	return result;
}

}
