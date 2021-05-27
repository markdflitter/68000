#ifndef FILE_H
#define FILE_H

#include <shared_ptr>
#include <string>
#include "Chunk.h"
#include "file_address_t.h"
#include <list>

class FAT;

class FileHeader
{
public:
	typedef mdf::shared_ptr <FileHeader> Ptr;
	typedef mdf::shared_ptr <const FileHeader> ConstPtr;

	FileHeader (FAT* fat = 0, const std::string& name = "", unsigned int index = 0, const std::list<Chunk::Ptr> chunks = std::list<Chunk::Ptr> ());

	FAT* fat ();
	void setFat (FAT* fat);

	std::string name () const;
	void setName (const std::string& name);

	unsigned int index () const;
	void setIndex (unsigned int index);

	bool bootable () const;
	void setBootable (bool bootable);

	file_address_t size () const;
	void setSize (file_address_t bytes);

	unsigned int loadAddress () const;
	void setLoadAddress (unsigned int loadAddress);

	unsigned int goAddress () const;
	void setGoAddress (unsigned int goAddress);

	std::list <Chunk::Ptr>& chunks ();
	const std::list <Chunk::Ptr>& chunks () const;
	void setChunks (const std::list <Chunk::Ptr>& chunks);

	file_address_t allocSize () const;
	
	void extend (std::list<Chunk::Ptr>& newAllocation);
private:
	FAT* m_fat;
	std::string m_name;
	unsigned int m_index;
	bool m_bootable;
	file_address_t  m_size;
	unsigned int m_loadAddress;
	unsigned int m_goAddress;
	std::list <Chunk::Ptr> m_chunks;
};

#endif
	
