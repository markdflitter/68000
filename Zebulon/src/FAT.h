#ifndef FAT_H
#define FAT_H

#include "SpaceManager.h"
#include "FileEntry.h"

namespace Zebulon
{

class FAT
{
public:
	int initialise (int diskSize);

	void load ();
	void save ();

	bool createFile (const std::string& name, unsigned long initialSize = 0, bool contiguous = false);
	void deleteFile (const std::string& name);

	bool extendFile (FileEntry::Ptr fileEntry, unsigned long numBlocks = 1);	

	void diag () const;
private:
	void serialise (unsigned char*& p) const;
	bool deserialise (const unsigned char*& p);

	void do_load ();
	void do_save () const;

	FileEntry::Ptr findFile (const std::string& name);

	SpaceManager m_spaceManager;	
	std::list<FileEntry::Ptr> m_fileEntries;
};

}

#endif
