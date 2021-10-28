#ifndef FILERGUARD_H
#define FILERGUARD_H

#include <shared_ptr>
#include <string>
#include "FileEntry.h"

namespace Zebulon
{

class Filer;

class FilerGuard
{
public:
	typedef mdf::shared_ptr <FilerGuard> Ptr;
	typedef mdf::shared_ptr <FilerGuard> ConstPtr;

	FilerGuard (Filer* filer);
	~FilerGuard ();

	int format (int diskSize);

	bool createFile (const std::string& name, unsigned long initialSize = 0, bool contiguous = false);
	bool deleteFile (const std::string& name);
	bool rightsizeFile (FileEntry::Ptr fileEntry, unsigned long totalBlocks);

	bool boot (unsigned int slot, const std::string& filename, unsigned int loadAddress, unsigned int startAddress, unsigned int length);

	void markModified ();
private:
	Filer* m_filer;
	bool m_modified;
};

}

#endif

