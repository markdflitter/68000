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

	void diag () const;
	FreeSpace getFreeSpace () const;
private:
	FAT m_FAT;
	std::vector<OpenFile::Ptr> m_openFiles;
};

}

#endif

