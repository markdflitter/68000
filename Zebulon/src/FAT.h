#ifndef FAT_H
#define FAT_H

#include "SpaceManager.h"
#include <size_t.h>

class FAT
{
public:
	FAT ();
	FAT (size_t size);

	unsigned int create_file (char* name, size_t size);
private:
 	SpaceManager m_spaceManager;
  
	struct File
	{
		char* _name;
	};
};

#endif
