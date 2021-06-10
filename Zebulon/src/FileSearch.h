#ifndef FILEFIND_H
#define FILEFIND_H

#include "FileHeader.h"
#include <shared_ptr>

class FileSearch
{
public:
	typedef mdf::shared_ptr <FileSearch> Ptr;
	typedef mdf::shared_ptr <const FileSearch> ConstPtr;

	FileSearch (const std::list<FileHeader::Ptr>& snapshot);
	
	FileHeader::Ptr next ();
private:
	std::list<FileHeader::Ptr> m_snapshot;
	std::list<FileHeader::Ptr>::iterator m_it;
};

#endif
