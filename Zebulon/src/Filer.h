#ifndef FILER_H
#define FILER_H

#include "FAT.h"
#include "OpenFile.h"
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

	file_handle fopen (const std::string& filename, const std::string& mode);
	void fclose (file_handle handle);
	bool feof (file_handle handle);
	unsigned long fwrite (file_handle handle, const unsigned char* data, unsigned long numBytes);
	unsigned long fread (file_handle handle, unsigned char* data, unsigned long numBytes);

	void diag () const;
	FreeSpace getFreeSpace () const;
private:
	OpenFile::Ptr getOpenFile (file_handle file);
	
	FAT m_FAT;
	std::vector<OpenFile::Ptr> m_openFiles;
};

}

#endif

