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

	struct File
	{
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
