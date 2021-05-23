#ifndef OPENFILE_H
#define OPENFILE_H

#include <shared_ptr>
#include "FileHeader.h"
#include "file_address.h"
#include "block_address.h"

class OpenFile
{
public:
	typedef mdf::shared_ptr <OpenFile> Ptr;
	typedef mdf::shared_ptr <OpenFile> ConstPtr;

	OpenFile (FileHeader::Ptr file);
	~OpenFile ();

	void read (unsigned char* data, file_address_t numBytes);	
	void write (unsigned char* data, file_address_t numBytes);	

	bool EOF () const;

	void flush ();
private:
	void writeCurBlock ();
	void readCurBlock ();
	bool findBlock (file_address_t filePointer, block_address_t & block);
		
	void setFilePointer (file_address_t filePointer);
	unsigned char* copyFromBuffer (unsigned char* data, file_address_t bytesToCopy);
	unsigned char* copyToBuffer (unsigned char* data, file_address_t bytesToCopy);
	
	unsigned char m_buffer [512];
	unsigned char* m_bufferPointer;
	
	FileHeader::Ptr m_fileHeader;

	file_address_t m_filePointer;
	block_address_t m_curBlock;
	bool m_bufferModified;
	bool m_bufferLoaded;
};

#endif
