#ifndef FAT_H
#define FAT_H

#include "SpaceManager.h"

namespace Zebulon
{

class FAT
{
public:
	void load ();
	int initialise (int diskSize);
private:
	void do_load ();
	void do_save () const;

	SpaceManager m_spaceManager;	
};

}

#endif
