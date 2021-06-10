#ifndef FAT_H
#define FAT_H

#include "block_address_t.h"
#include <string>
#include "file_handle.h"
#include <list>

#include "FileHeader.h"
#include "SpaceManager.h"
#include <vector>
#include "OpenFile.h"
#include "BootTableEntry.h"
#include "file_search_handle.h"
#include "FileSearch.h"
#include <Zebulon.h>

class FAT
{
public:
	FAT ();

	size_t blockSize () const;

	bool readBlock (block_address_t block, unsigned char* data);

	void format (block_address_t size);
	
	bool fileExists (const std::string& name);

	bool create (const std::string& name, block_address_t initialSize = 0, bool contiguous = false);
	void setMetaData (const std::string& name, unsigned int loadAddress, unsigned int goAddress);

	bool stat (const std::string& name, struct Zebulon::_zebulon_stat* s);

	file_search_handle findFirstFile (struct Zebulon::_zebulon_stat* s);
	bool findNextFile (file_search_handle& handle, struct Zebulon::_zebulon_stat* s);
	void closeFind (file_search_handle& handle);

	void rm (const std::string& name);

	file_handle open (const std::string& name);
	void close (file_handle file);

	file_address_t read (file_handle file, unsigned char* data, file_address_t numBytes) const;
	file_address_t write (file_handle file, const unsigned char* data, file_address_t numBytes);

	bool EOF (file_handle file) const;

	void boot (const std::string& name, unsigned int index);
	void unboot (unsigned int index);
	void index () const;
	
	bool extend (FileHeader::Ptr fileHeader, block_address_t numBlocks = 1);	void save () const;
private:
	FileHeader::Ptr findFile (const std::string& name);

	OpenFile::ConstPtr getOpenFile (file_handle file) const;
	OpenFile::Ptr getOpenFile (file_handle file);

	FileSearch::Ptr getFileSearch (file_search_handle handle);

	void serialise (unsigned char*& p) const;
	bool deserialise (const unsigned char*& p);

	void load ();

	SpaceManager m_spaceManager;	
	std::list<FileHeader::Ptr> m_fileHeaders;
	std::vector<OpenFile::Ptr> m_openFiles;
	std::vector<FileSearch::Ptr> m_fileSearches;
	std::vector<BootTableEntry::Ptr> m_bootTable;
	
	static unsigned int m_lastIndex;
};

#endif
