#ifndef OPENFILE_H
#define OPENFILE_H

#include <shared_ptr>
#include "FileEntry.h"
#include "FilerGuard.h"
#include <bsp.h>

namespace Zebulon
{

class OpenFile
{
public:
	typedef mdf::shared_ptr <OpenFile> Ptr;
	typedef mdf::shared_ptr <OpenFile> ConstPtr;

	OpenFile (FileEntry::Ptr file, FilerGuard::Ptr filerGuard);
	~OpenFile ();

	unsigned long read (unsigned char* data, unsigned long numBytes);	
	unsigned long write (const unsigned char* data, unsigned long numBytes);	

	bool EOF () const;

	void flush ();
private:
	void writeCurBlock ();
	bool readCurBlock ();
	unsigned long findBlock (unsigned long filePointer) const;
		
	void setFilePointer (unsigned long filePointer);
	unsigned char* copyFromBuffer (unsigned char* data, unsigned long bytesToCopy);
	const unsigned char* copyToBuffer (const unsigned char* data, unsigned long bytesToCopy);
	
	unsigned char m_buffer [ide_block_size];
	unsigned char* m_bufferPointer;
	
	FileEntry::Ptr m_fileEntry;
	FilerGuard::Ptr m_filerGuard;

	unsigned long m_filePointer;
	unsigned long m_curBlock;
	bool m_bufferModified;
	bool m_bufferLoaded;
};

}
#endif
