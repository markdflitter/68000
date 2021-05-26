#ifndef FILESTAT_H
#define FILESTAT_H

#include <string>
#include "file_address_t.h"
#include <list>
#include "Chunk.h"

struct FileStat
{
	FileStat () {}
	FileStat (const std::string& _name, unsigned int _index, bool _bootable, file_address_t _size, file_address_t _allocSize)
		: name (_name), index (_index), bootable (_bootable), size (_size), allocSize (_allocSize) {}

	std::string name;
	unsigned int index;
	bool bootable;
	file_address_t size;	
	file_address_t allocSize;

	std::list <Chunk> chunks;
};

#endif
