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
	FAT (size_t size);

	struct File
	{
		//std::string m_name;
		std::list <SpaceManager::Chunk> m_chunks;
	};

	File createFile (const std::string& name, size_t size);
	std::list<File> ls () const;
private:
	void load ();
	void save ();

 	SpaceManager m_spaceManager;
	
	std::list<File> m_files;
};

#endif
