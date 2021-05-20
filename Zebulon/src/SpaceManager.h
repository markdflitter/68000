#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include <size_t.h>
#include <list>

class SpaceManager
{
public:
	SpaceManager ();	
	SpaceManager (size_t size);	

	struct Chunk {
		Chunk () : m_start (0), m_length (0) {} 
		Chunk (unsigned int start, size_t length) : m_start (start), m_length (length) {} 
		unsigned int m_start;
		size_t m_length;
	};

	std::list <Chunk> allocate (size_t size);

	void print () const;
private:
	void format (size_t size);

	void load ();
	void save () const;
	
	std::list<Chunk> m_free;
};

#endif
