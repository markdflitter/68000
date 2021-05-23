#ifndef FAT_H
#define FAT_H

#include <string>
#include "File.h"
#include "block_address.h"
#include "SpaceManager.h"
#include "OpenFile.h"
#include <list>

class FAT
{
public:
	FAT ();

	File createFile (const std::string& name, size_t size);
	OpenFile open (const File& file);

	std::list<File>& ls ();

	void format (block_address_t size);

	void save () const;
private:
	unsigned char* serialise (unsigned char* p) const;
	unsigned char* deserialise (unsigned char* p);

	void load ();
 
	SpaceManager m_spaceManager;	
	std::list<File> m_files;
};

#endif
