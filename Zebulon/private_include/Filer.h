#ifndef FILER_H
#define FILER_H

#include <shared_ptr>
#include "FAT.h"
#include "BootTable.h"
#include "OpenFile.h"
#include "FileSearch.h"
#include <vector>
#include "FilerGuard.h"

namespace Zebulon
{

class Filer
{
	friend class FilerGuard;
public:
	Filer ();
	~Filer ();

	void load ();
	void save ();

	FilerGuard::Ptr guard () {return mdf::make_shared (new FilerGuard (this));}
	
	typedef int file_handle;
	enum {file_not_found = -1};

	file_handle fopen (const std::string& name, const std::string& mode);
	void fclose (file_handle handle);
	bool feof (file_handle handle);
	unsigned long fwrite (file_handle handle, const unsigned char* data, unsigned long numBytes);
	unsigned long fread (file_handle handle, unsigned char* data, unsigned long numBytes);

	int statFile (const std::string& name, _zebulon_stats* stats);	

	typedef int file_search_handle;	
	file_search_handle findFirstFile (char filename [FILENAME_BUFFER_SIZE]);
	bool findNextFile (file_search_handle handle, char filename [FILENAME_BUFFER_SIZE]);
	void closeFind (file_search_handle handle);

	void index (_zebulon_boot_table_entry btes[9]);

	void diag () const;
	_zebulon_free_space getFreeSpace () const;
private:
	template <class T>
    	int findFirstFreeHandle (const T& vec) const;

	void do_load ();
	bool do_load_bootTable ();
	bool do_load_FAT ();

	void do_save ();
	void do_save_bootTable ();
	void do_save_FAT ();

	OpenFile::Ptr getOpenFile (file_handle file);
	FileSearch::Ptr getFileSearch (file_search_handle handle);

	int format (int diskSize);

	bool createFile (const std::string& name, unsigned long initialSize = 0, bool contiguous = false);
	bool deleteFile (const std::string& name);

	bool rightsizeFile (FileEntry::Ptr fileEntry, unsigned long totalBlocks);	

	bool boot (unsigned int slot, const std::string& filename, unsigned int loadAddress, unsigned int startAddress, unsigned int length);
	
	bool m_initialised;
	FAT m_FAT;
	BootTable m_bootTable;
	std::vector<OpenFile::Ptr> m_openFiles;
	std::vector<FileSearch::Ptr> m_fileSearches;
};

}

#endif

