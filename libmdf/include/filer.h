#ifndef FILER_H
#define FILER_H

#include <string>

namespace mdf
{

struct stat
{
	unsigned int index;

	std::string name;

	long unsigned int size;	
	long unsigned int sizeOnDisk;	

	bool bootable;
	unsigned int loadAddress;
	unsigned int goAddress;
};

bool stat (const std::string& name, struct stat* s);

int findFirstFile (struct stat* s);
bool findNextFile (int handle, struct stat* s);
void closeFind (int handle);

void deleteFile (const std::string& filename);

void boot (const std::string& filename, unsigned int bootSlot);
void unboot (unsigned int bootSlot);

struct bootTableEntry
{
	bool empty;

	unsigned int index;
 	char name [20];
	unsigned int file_index;

	long unsigned int size;	
	unsigned int loadAddress;
	unsigned int goAddress;

	unsigned int startBlock;
};

void index (bootTableEntry entries [10]);

void format (long unsigned int blocks);
void dumpBlock (long unsigned int block);

}

#endif

