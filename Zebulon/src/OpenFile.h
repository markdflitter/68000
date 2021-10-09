#ifndef OPENFILE_H
#define OPENFILE_H

#include <shared_ptr>
#include "FileEntry.h"

namespace Zebulon
{

class FAT;

class OpenFile
{
public:
	typedef mdf::shared_ptr <OpenFile> Ptr;
	typedef mdf::shared_ptr <OpenFile> ConstPtr;

	OpenFile (FileEntry::Ptr file, FAT* fat);
	~OpenFile ();

	void read (unsigned char* data, unsigned long numBytes);	
	void write (const unsigned char* data, unsigned long numBytes);	

	bool EOF () const;

	void flush ();
private:
	void writeCurBlock ();
	bool readCurBlock ();
	unsigned long findBlock (unsigned long filePointer) const;
		
	void setFilePointer (unsigned long filePointer);
	unsigned char* copyFromBuffer (unsigned char* data, unsigned long bytesToCopy);
	const unsigned char* copyToBuffer (const unsigned char* data, unsigned long bytesToCopy);
	
	unsigned char m_buffer [512];
	unsigned char* m_bufferPointer;
	
	FileEntry::Ptr m_fileEntry;
	FAT* m_FAT;

	unsigned long m_filePointer;
	unsigned long m_curBlock;
	bool m_bufferModified;
	bool m_bufferLoaded;
	bool m_fileEntryModified;
};

}
#endif
