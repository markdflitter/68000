#include "../include/filer.h"
#include "Zebulon.h"
#include <string.h>
#include <string>

using namespace std;
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

void convert_btes (const _zebulon_boot_table_entry zbte [10], mdf::bootTableEntry bte [10])
{ 
	for (int i = 0; i < 10; i++)
	{
		bte [i].empty = zbte [i].empty;
		bte [i].index = zbte [i].index;
		memcpy (bte [i].name, zbte [i].name, 20);
		bte [i].file_index = zbte [i].file_index;
		bte [i].index = zbte [i].size;
		bte [i].loadAddress = zbte [i].loadAddress;
		bte [i].goAddress = zbte [i].goAddress;
		bte [i].startBlock = zbte [i].startBlock;
	}
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

void deleteFile (const std::string& filename)
{
	_zebulon_delete_file (filename.c_str ());
}

void save (const string& filename, unsigned int bootSlot)
{
	_zebulon_save (filename.c_str (), bootSlot);
}

void boot (const std::string& filename, unsigned int bootSlot)
{
	_zebulon_boot (filename.c_str (), bootSlot);
}

void unboot (unsigned int bootSlot)
{
	_zebulon_unboot (bootSlot);
}

void index (bootTableEntry entries [10])
{
	_zebulon_boot_table_entry bte [10];
	_zebulon_index (bte);
	convert_btes (bte,  entries);
}

void format (long unsigned int blocks)
{
	_zebulon_format (blocks);
}

}
