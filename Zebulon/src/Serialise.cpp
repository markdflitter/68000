#include "Serialise.h"
#include <string.h>

using namespace std;

namespace
{

unsigned char* inc_p (unsigned char* p, size_t sz)
{
	return p + sz + (sz & 0x1 ? 1 : 0);
}

unsigned char* copyFrom (void* dest, const void* src, size_t numBytes)
{
	memcpy (dest, src, numBytes);
	return inc_p ((unsigned char*) src, numBytes);
}

unsigned char* copyTo (void* dest, const void* src, size_t numBytes)
{
	memcpy (dest, src, numBytes);
	return inc_p ((unsigned char*) dest, numBytes);
}

}

unsigned char* Serialise::serialise (unsigned long l, unsigned char* p)
{
	size_t sz = sizeof (l);
	return copyTo (p, &l, sz); 
}

unsigned char* Serialise::serialise (const string& s, unsigned char* p)
{
	size_t sz = s.length ();
	p = serialise (sz, p);	
	return copyTo (p, s.c_str (), sz);
}

unsigned char* Serialise::serialise (Chunk::Ptr chunk, unsigned char* p)
{
	size_t sz = sizeof (*chunk);
	return copyTo (p, chunk.get_raw (), sz);
}

unsigned char* Serialise::serialise (FileHeader::Ptr file, unsigned char* p)
{
	p = serialise (file->name (), p);
	p = serialise ((unsigned long) file->index (), p);
	p = serialise ((unsigned long) file->bootable (), p);
	p = serialise (file->size (), p);
	p = serialise (file->chunks (), p);

	return p;
}

unsigned char* Serialise::serialise (BootTableEntry::Ptr bte, unsigned char* p)
{
	if (bte.isNull ()) bte = mdf::make_shared (new BootTableEntry ());

	p = serialise ((unsigned long) bte->empty, p);
	p = serialise (string (bte->shortName), p);
	p = serialise ((unsigned long) bte->index, p);
	p = serialise ((unsigned long) bte->length, p);
	p = serialise ((unsigned long) bte->loadAddress, p);
	p = serialise ((unsigned long) bte->goAddress, p);
	p = serialise ((unsigned long) bte->startBlock, p);
	
	return p;
}

unsigned char* Serialise::serialise (Chunk::ConstPtr chunk, unsigned char* p)
{
	size_t sz = sizeof (*chunk);
	return copyTo (p, chunk.get_raw (), sz);
}

unsigned char* Serialise::serialise (FileHeader::ConstPtr file, unsigned char* p)
{
	p = serialise (file->name (), p);
	p = serialise (file->size (), p);
	p = serialise (file->chunks (), p);

	return p;
}


unsigned char* Serialise::deserialise (unsigned long& l, unsigned char* p)
{
	size_t sz = sizeof (l);
	return copyFrom (&l, p, sz); 
}

unsigned char* Serialise::deserialise (string& s, unsigned char* p, size_t maxLength)
{
	size_t sz = 0;
	p =	deserialise (sz, p);
	if (sz > maxLength) sz = maxLength;

	char* buf = new char [sz + 1];
	buf [sz] = '\0';

	p = copyFrom (buf, p, sz);
	
	s = string (buf);
				
	return p;
}

unsigned char* Serialise::deserialise (Chunk::Ptr chunk, unsigned char* p)
{
	size_t sz = sizeof (*chunk);
	return copyFrom (chunk.get_raw (), p, sz);
}

unsigned char* Serialise::deserialise (FileHeader::Ptr file, unsigned char* p)
{
	string name;	
	p = deserialise (name, p, 255);
	file->setName (name);

	unsigned long index;
	p = deserialise (index, p);
	file->setIndex ((unsigned int) index);

	unsigned long bootable;
	p = deserialise (bootable, p);
	file->setBootable ((bool) bootable);
	
	file_address_t size;	
	p = deserialise (size, p);
	file->setSize (size);

	list<Chunk::Ptr> chunks;
	p = deserialise (chunks, p);
	file->setChunks (chunks);

	return p;
}

unsigned char* Serialise::deserialise (BootTableEntry::Ptr bte, unsigned char* p)
{
	unsigned long empty;
	p = deserialise (empty, p);
	bte->empty = empty;

	string name;
	p = deserialise (name, p, 21);
	memcpy (bte->shortName, name.c_str (), name.length () + 1);
	
	unsigned long index;
	p = deserialise (index, p);
	bte->index = index;
	
	unsigned long length;
	p = deserialise (length, p);
	bte->length = length;

	unsigned long loadAddress;
	p = deserialise (loadAddress, p);
	bte->loadAddress = loadAddress;

	unsigned long goAddress;
	p = deserialise (goAddress, p);
	bte->goAddress = goAddress;

	unsigned long startBlock;
	p = deserialise (startBlock, p);
	bte->startBlock = startBlock;

	return p;
}
