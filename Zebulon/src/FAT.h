#ifndef FAT_H
#define FAT_H

#include "SpaceManager.h"
#include <size_t.h>
#include <list>
#include <string>

class FAT
{
public:
	FAT ();

	class File;

	class OpenFile
	{
	public:
		OpenFile (File& file);
		~OpenFile ();

		void read (unsigned char* data, size_t numBytes);	
		void write (unsigned char* data, size_t numBytes);	

		bool EOF () const;

		void flush ();
	private:
		void writeCurBlock ();
		void readCurBlock ();
		bool findBlock (size_t filePointer, unsigned long& block);
		
		void setFilePointer (size_t filePointer);
		unsigned char* copyFromBuffer (unsigned char* data, size_t bytesToCopy);
		unsigned char* copyToBuffer (unsigned char* data, size_t bytesToCopy);
	
		File& m_file;
		unsigned char m_buffer [512];
		unsigned char* m_bufferPointer;
		
		size_t m_filePointer;
		unsigned long m_curBlock;
		bool m_bufferModified;
		bool m_bufferLoaded;
	};

	class File
	{
	public:
		File ();
		File (const std::string& name);

		std::string& name ();
	size_t& bytes ();
		std::list <SpaceManager::Chunk>& chunks ();

		const std::string& name () const;
		size_t allocSize () const;
		const size_t& bytes () const;
		const std::list <SpaceManager::Chunk>& chunks () const;

		FAT::OpenFile open ();
		FAT* fat;
	private:
		std::string m_name;
		std::list <SpaceManager::Chunk> m_chunks;
		size_t m_bytes;
	};

	File createFile (const std::string& name, size_t size);
	std::list<File>& ls ();

	void format (size_t size);

	void save () const;
private:
	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);

	void load ();
 	SpaceManager m_spaceManager;
	
	std::list<File> m_files;
};

#endif
