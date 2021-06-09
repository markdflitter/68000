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

}

#endif

