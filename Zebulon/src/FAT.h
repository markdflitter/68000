#ifndef FAT_H
#define FAT_H

#include "SpaceManager.h"
#include <size_t.h>
#include <list>
#include <string>

class FAT
{
public:
	typedef unsigned long file_address_t;
	
	FAT ();

	class File;

	class OpenFile
	{
	public:
		OpenFile (File& file);
		~OpenFile ();

		void read (unsigned char* data, file_address_t numBytes);	
		void write (unsigned char* data, file_address_t numBytes);	

		bool EOF () const;

		void flush ();
	private:
		void writeCurBlock ();
		void readCurBlock ();
		bool findBlock (file_address_t filePointer, SpaceManager::block_address_t & block);
		
		void setFilePointer (file_address_t filePointer);
		unsigned char* copyFromBuffer (unsigned char* data, file_address_t bytesToCopy);
		unsigned char* copyToBuffer (unsigned char* data, file_address_t bytesToCopy);
	
		File& m_file;
		unsigned char m_buffer [512];
		unsigned char* m_bufferPointer;
		
		file_address_t m_filePointer;
		SpaceManager::block_address_t m_curBlock;
		bool m_bufferModified;
		bool m_bufferLoaded;
	};

	class File
	{
	public:
		File ();
		File (const std::string& name);

		std::string name () const;
		void setName (const std::string& name);

		file_address_t size () const;
		void setSize (file_address_t bytes);

		std::list <SpaceManager::Chunk>& chunks ();

		file_address_t allocSize () const;
		const std::list <SpaceManager::Chunk>& chunks () const;

		FAT::OpenFile open ();
		FAT* fat;
	private:
		std::string m_name;
		file_address_t  m_size;
		std::list <SpaceManager::Chunk> m_chunks;
	};

	File createFile (const std::string& name, size_t size);
	std::list<File>& ls ();

	void format (SpaceManager::block_address_t size);

	void save () const;
private:
	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);

	void load ();
 	SpaceManager m_spaceManager;
	
	std::list<File> m_files;
};

#endif
