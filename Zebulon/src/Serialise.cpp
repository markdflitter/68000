#include "../private_include/Serialise.h"
#include <string.h>

using namespace std;

namespace
{

template <class T>
void inc_p (T*& p, size_t sz)
{
	p = p + sz + (sz & 0x1 ? 1 : 0);
}

void copyFrom (unsigned char* dest, const unsigned char*& src, size_t numBytes)
{
	memcpy (dest, src, numBytes);
	inc_p (src, numBytes);
}

void copyTo (unsigned char*& dest, const unsigned char* src, size_t numBytes)
{
	memcpy (dest, src, numBytes);
	inc_p (dest, numBytes);
}

}

namespace Zebulon
{

void Serialise::serialise (unsigned long l, unsigned char*& p)
{
	size_t sz = sizeof (l);
	copyTo (p, (const unsigned char*) &l, sz); 
}

void Serialise::serialise (const string& s, unsigned char*& p)
{
	size_t sz = s.length ();
	serialise (sz, p);	
	copyTo (p, (const unsigned char*) s.c_str (), sz);
}

void Serialise::serialise (Chunk::Ptr chunk, unsigned char*& p)
{
	size_t sz = sizeof (*chunk);
	copyTo (p, (const unsigned char*) chunk.get_raw (), sz);
}

void Serialise::serialise (BootTableEntry::Ptr bte, unsigned char*& p)
{
	if (bte.isNull ()) bte = mdf::make_shared (new BootTableEntry ());

	serialise ((unsigned long) bte->empty, p);
	serialise (bte->shortName, p);
	serialise ((unsigned long) 0, p);
	serialise ((unsigned long) bte->length, p);
	serialise ((unsigned long) bte->loadAddress, p);
	serialise ((unsigned long) bte->goAddress, p);
	serialise ((unsigned long) bte->startBlock, p);
}

void Serialise::serialise (FileEntry::Ptr file, unsigned char*& p)
{
	serialise (file->name (), p);
	serialise (file->size (), p);
	serialise (file->chunks (), p);
}


void Serialise::deserialise (unsigned long& l, const unsigned char*& p)
{
	size_t sz = sizeof (l);
	copyFrom ((unsigned char*) &l, p, sz); 
}

void Serialise::deserialise (string& s, const unsigned char*& p, size_t maxLength)
{
	size_t sz = 0;
	deserialise (sz, p);
	if (sz > maxLength) sz = maxLength;

	char* buf = new char [sz + 1];
	buf [sz] = '\0';

	copyFrom ((unsigned char*) buf, p, sz);
	
	s = string (buf);
}

void Serialise::deserialise (Chunk::Ptr chunk, const unsigned char*& p)
{
	size_t sz = sizeof (*chunk);
	copyFrom ((unsigned char*) chunk.get_raw (), p, sz);
}

void Serialise::deserialise (FileEntry::Ptr file, const unsigned char*& p)
{
	string name;	
	deserialise (name, p, 255);
	file->setName (name);

	unsigned long size;	
	deserialise (size, p);
	file->setSize (size);

	list<Chunk::Ptr> chunks;
	deserialise (chunks, p);
	file->setChunks (chunks);
}

void Serialise::deserialise (BootTableEntry::Ptr bte, const unsigned char*& p)
{
	unsigned long empty;
	deserialise (empty, p);
	bte->empty = empty;

	string name;
	deserialise (name, p, 20);
	bte->shortName = name;
	
	unsigned long ignore;
	deserialise (ignore, p);

	unsigned long length;
	deserialise (length, p);
	bte->length = length;

	unsigned long loadAddress;
	deserialise (loadAddress, p);
	bte->loadAddress = loadAddress;

	unsigned long goAddress;
	deserialise (goAddress, p);
	bte->goAddress = goAddress;

	unsigned long startBlock;
	deserialise (startBlock, p);
	bte->startBlock = startBlock;
}

}
