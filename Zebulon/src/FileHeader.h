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

	FileHeader (FAT* fat = 0, const std::string& name = "", const std::list<Chunk::Ptr> chunks = std::list<Chunk::Ptr> ());

	const FAT* fat () const;
	void setFat (const FAT* fat);

	std::string name () const;
	void setName (const std::string& name);

	file_address_t size () const;
	void setSize (file_address_t bytes);

	const std::list <Chunk::Ptr>& chunks () const;
	void setChunks (const std::list <Chunk::Ptr>& chunks);

	file_address_t allocSize () const;
private:
	const FAT* m_fat;
	std::string m_name;
	file_address_t  m_size;
	std::list <Chunk::Ptr> m_chunks;
};

#endif
	
