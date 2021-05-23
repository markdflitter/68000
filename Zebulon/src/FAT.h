#ifndef FAT_H
#define FAT_H

#include "block_address.h"
#include <string>
#include "file_handle.h"
#include <list>
#include "FileHeader.h"
#include "SpaceManager.h"
#include <vector>
#include "OpenFile.h"

class FAT
{
public:
	FAT ();
	~FAT ();
		
	void format (block_address_t size);
	
	void create (const std::string& name, block_address_t initialSize);
	FILE open (const std::string& name);
	void close (FILE file);

	std::list<FileHeader::Ptr>& ls ();

	void save () const;
private:
	FileHeader::Ptr findFile (const std::string& name);

	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);

	void load ();

	SpaceManager m_spaceManager;	
	std::list<FileHeader::Ptr> m_fileHeaders;
	std::vector<OpenFile::Ptr> m_openFiles;
};

#endif
