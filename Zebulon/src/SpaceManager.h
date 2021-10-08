#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include <list>
#include "Chunk.h"

namespace Zebulon
{

class SpaceManager
{
public:
	void load ();
	int initialise (int diskSize);
private:
	void do_load ();
	void do_save () const;
	
	std::list<Chunk::Ptr> m_free;
};

}
#endif
