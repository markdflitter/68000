#include "../include/filer.h"
#include "Zebulon.h"

using namespace Zebulon;

namespace mdf
{

bool stat (const std::string& name, struct stat* s)
{
	struct _zebulon_stat zs;
	bool result = _zebulon_stat (name.c_str (), &zs);

	if (result)
	{
		s->name = std::string (zs.name);
		s->size = zs.size;
	}

	return result;
}

int findFirstFile (struct stat* s)
{
	struct _zebulon_stat zs;
	int result = _zebulon_find_first (&zs)

	if (result > -1)
	{
		s->name = std::string (zs.name);
		s->size = zs.size;
	}

	return result;
}

bool findNextFile (int handle, struct stat* s)
{
	struct _zebulon_stat zs;
	bool result = _zebulon_find_next (handle, &zs)

	if (result)
	{
		s->name = std::string (zs.name);
		s->size = zs.size;
	}

	return result;
}

void closeFind (int handle)
{
	_zebulon_close_find (handle);
}

}
