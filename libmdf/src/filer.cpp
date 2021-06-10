#include "../include/filer.h"
#include "Zebulon.h"

using namespace Zebulon;

namespace
{

void convertStat (struct Zebulon::_zebulon_stat zs, struct mdf::stat* result)
{
	result->index = zs.index;

	result->name = std::string (zs.name);
	result->size = zs.size;

	result->sizeOnDisk = zs.sizeOnDisk;
	
	result->bootable = zs.bootable;
	result->loadAddress = zs.loadAddress;
	result->goAddress = zs.goAddress;
}

}


namespace mdf
{

bool stat (const std::string& name, struct stat* s)
{
	struct _zebulon_stat zs;
	bool result = _zebulon_stat (name.c_str (), &zs);

	if (result) convertStat (zs, s);

	return result;
}

int findFirstFile (struct stat* s)
{
	struct _zebulon_stat zs;
	int result = _zebulon_find_first_file (&zs);

	if (result > -1) convertStat (zs, s);

	return result;
}

bool findNextFile (int handle, struct stat* s)
{
	struct _zebulon_stat zs;
	bool result = _zebulon_find_next_file (handle, &zs);

	if (result) convertStat (zs, s);

	return result;
}

void closeFind (int handle)
{
	_zebulon_close_find (handle);
}

}
