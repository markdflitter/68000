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

}

#endif

