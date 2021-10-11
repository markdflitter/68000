#ifndef FILESEARCH_H
#define FILESEARCH_H

#include "FileEntry.h"
#include <shared_ptr>
#include <list>

namespace Zebulon
{

class FileSearch
{
public:
	typedef mdf::shared_ptr <FileSearch> Ptr;
	typedef mdf::shared_ptr <const FileSearch> ConstPtr;

	FileSearch (const std::list<FileEntry::Ptr>& snapshot);
	
	FileEntry::Ptr next ();
private:
	std::list<FileEntry::Ptr> m_snapshot;
	std::list<FileEntry::Ptr>::iterator m_it;
};

}
#endif
