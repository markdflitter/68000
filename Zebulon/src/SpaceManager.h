#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include <list>
#include "Chunk.h"

namespace Zebulon
{

class SpaceManager
{
public:
	int initialise (int initialSize);

	void serialise (unsigned char*& p) const;
	void deserialise (const unsigned char*& p);
private:	
	std::list<Chunk::Ptr> m_free;
};

}
#endif
