#ifndef FILER_H
#define FILER_H

#include "FAT.h"
#include "OpenFile.h"
#include "FileSearch.h"
#include <vector>

namespace Zebulon
{

class Filer
{
public:
	Filer ();
	~Filer ();

	void load();
	int format (int diskSize);

	typedef int file_handle;
	enum {file_not_found = -1};

	file_handle fopen (const std::string& name, const std::string& mode);
	void fclose (file_handle handle);
	bool feof (file_handle handle);
	unsigned long fwrite (file_handle handle, const unsigned char* data, unsigned long numBytes);
	unsigned long fread (file_handle handle, unsigned char* data, unsigned long numBytes);

	int statFile (const std::string& name, _zebulon_stats* stats);	
	bool deleteFile (const std::string& name);

	typedef int file_search_handle;	
	file_search_handle findFirstFile (char filename [FILENAME_BUFFER_SIZE]);
	bool findNextFile (file_search_handle handle, char filename [FILENAME_BUFFER_SIZE]);
	void closeFind (file_search_handle handle);

	void diag () const;
	_zebulon_free_space getFreeSpace () const;
private:
	OpenFile::Ptr getOpenFile (file_handle file);
	FileSearch::Ptr getFileSearch (file_search_handle handle);

	FAT m_FAT;
	std::vector<OpenFile::Ptr> m_openFiles;
	std::vector<FileSearch::Ptr> m_fileSearches;
};

}

#endif

