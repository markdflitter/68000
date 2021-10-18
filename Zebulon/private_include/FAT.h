#ifndef FAT_H
#define FAT_H

#include "SpaceManager.h"
#include "FileEntry.h"
#include "FileSearch.h"

namespace Zebulon
{

class FAT
{
public:
	int initialise (int diskSize);

	void serialise (unsigned char*& p) const;
	bool deserialise (const unsigned char*& p);

	FileEntry::Ptr findFile (const std::string& name);

	bool createFile (const std::string& name, unsigned long initialSize = 0, bool contiguous = false);
	bool deleteFile (const std::string& name);

	bool extendFile (FileEntry::Ptr fileEntry, unsigned long numBlocks = 1);	

	int statFile (const std::string& name, _zebulon_stats* stats);

	FileSearch::Ptr findFirstFile ();

	void diag () const;
	_zebulon_free_space getFreeSpace () const;
private:
	SpaceManager m_spaceManager;	
	std::list<FileEntry::Ptr> m_fileEntries;
};

}

#endif
