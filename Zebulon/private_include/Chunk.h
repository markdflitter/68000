#ifndef CHUNK_H
#define CHUNK_H

#include <shared_ptr>

struct Chunk {
	typedef mdf::shared_ptr <Chunk> Ptr;
	typedef mdf::shared_ptr <const Chunk> ConstPtr;

	Chunk () : start (0), length (0) {} 
	Chunk (unsigned long start, unsigned long length) : start (start), length (length) {} 

	unsigned start;
	unsigned length;
};

#endif
