#ifndef FILE_H
#define FILE_H

#include <shared_ptr>
#include <string>
#include "SpaceManager.h"
#include <list>

namespace Zebulon
{

class FileEntry
{
public:
	typedef mdf::shared_ptr <FileEntry> Ptr;
	typedef mdf::shared_ptr <const FileEntry> ConstPtr;

	FileEntry (const std::string& name = "");

	const std::string& name () const;
	void setName (const std::string& name);

	unsigned long size () const;
	void setSize (unsigned long bytes);

	unsigned long allocSize () const;
	
	std::list <Chunk::Ptr>& chunks ();
	const std::list <Chunk::Ptr>& chunks () const;
	void setChunks (const std::list <Chunk::Ptr>& chunks);

	bool extend (SpaceManager& sm, unsigned int blocksToAdd, bool contiguous);
	void truncate (SpaceManager& sm, unsigned int blocksToFree);

	void diag () const;
private:
	void consolidate ();

	std::string m_name;
	unsigned int m_size;
	std::list <Chunk::Ptr> m_chunks;
};

}
#endif
	
