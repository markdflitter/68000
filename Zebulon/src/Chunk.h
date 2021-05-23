#ifndef CHUNK_H
#define CHUNK_H

#include <shared_ptr>
#include "block_address.h"

struct Chunk {
	typedef mdf::shared_ptr <Chunk> Ptr;
	typedef mdf::shared_ptr <const Chunk> ConstPtr;

	Chunk () : start (0), length (0) {} 
	Chunk (block_address_t start, block_address_t length) : start (start), length (length) {} 

	block_address_t start;
	block_address_t length;
};

#endif
