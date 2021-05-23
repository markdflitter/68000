#ifndef FILESTAT_H
#define FILESTAT_H

#include <string>
#include "file_address_t.h"
#include <list>
#include "Chunk.h"

struct FileStat
{
	FileStat () {}
	FileStat (const std::string& _name, file_address_t _size, file_address_t allocSize)
		: name (_name), size (_size) {}

	std::string name;
	file_address_t size;	
	file_address_t allocSize;

	std::list <Chunk> chunks;
};

#endif
