#ifndef FILER_H
#define FILER_H

#include <string>

namespace mdf
{

struct stat
{
	unsigned long int size;
	std::string name;
};

bool stat (const std::string& name, struct stat* s);

int findFirstFile (struct stat* s);
bool findNextFile (int handle, struct stat* s);
void closeFind (int handle);

}

#endif

