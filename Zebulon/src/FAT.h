#ifndef FAT_H
#define FAT_H

#include "block_address_t.h"
#include <string>
#include "file_handle.h"
#include <list>
#include "FileStat.h"

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
	
	void create (const std::string& name, block_address_t initialSize = 0);

	FILE open (const std::string& name);
	void close (FILE file);

	void read (FILE file, unsigned char* data, file_address_t numBytes) const;
	void write (FILE file, unsigned char* data, file_address_t numBytes);

	bool EOF (FILE file) const;

	std::list<std::string> ls () const;
	FileStat stat (const std::string& name) const;

	void extend (FileHeader::Ptr fileHeader, block_address_t numBlocks = 1);	
	void save () const;
private:
	FileHeader::ConstPtr findFile (const std::string& name) const;
	FileHeader::Ptr findFile (const std::string& name);
	OpenFile::ConstPtr getOpenFile (FILE file) const;
	OpenFile::Ptr getOpenFile (FILE file);


	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);

	void load ();

	SpaceManager m_spaceManager;	
	std::list<FileHeader::Ptr> m_fileHeaders;
	std::vector<OpenFile::Ptr> m_openFiles;
};

#endif
