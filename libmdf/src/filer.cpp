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

}
