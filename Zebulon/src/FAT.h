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
		OpenFile (const File& file);
		~OpenFile ();

		void read (unsigned char* data, size_t numBytes);	
		void write (unsigned char* data, size_t numBytes);	

		void flush ();
	private:
		static void writeBlock (unsigned long block, unsigned char* buffer);
		static void readBlock (unsigned long block, unsigned char* buffer);
		bool block (size_t filePointer, unsigned long& block);
		
		void setFilePointer (size_t filePointer);
		unsigned char* copyFromBuffer (unsigned char* data, size_t bytesToCopy);
		unsigned char* copyToBuffer (unsigned char* data, size_t bytesToCopy);


		const File& m_file;
		unsigned char m_buffer [512];
		unsigned char* m_bufferPointer;
	
		size_t m_filePointer;
		unsigned long m_curBlock;
		bool m_bufferModified;
	};

	class File
	{
	public:
		File ();
		File (const std::string& name);

		std::string& name ();
		std::list <SpaceManager::Chunk>& chunks ();

		const std::string& name () const;
		const std::list <SpaceManager::Chunk>& chunks () const;

		FAT::OpenFile open ();
	private:
		std::string m_name;
		std::list <SpaceManager::Chunk> m_chunks;
	};

	File createFile (const std::string& name, size_t size);
	std::list<File> ls () const;

	void format (size_t size);
private:
	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);

	void load ();
	void save () const;

 	SpaceManager m_spaceManager;
	
	std::list<File> m_files;
};

#endif
