#include "../private_include/Serialise.h"
#include <string.h>
#include <stdio.h>

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

void copyTo (unsigned char*& dest, const unsigned char* src, size_t numBytes, bool sizeOnly)
{
	if (!sizeOnly) memcpy (dest, src, numBytes);
	inc_p (dest, numBytes);
}

}

namespace Zebulon
{

void Serialise::serialise (unsigned long l, unsigned char*& p, bool sizeOnly)
{
	size_t sz = sizeof (l);
	copyTo (p, (const unsigned char*) &l, sz, sizeOnly); 
}

void Serialise::serialise (const string& s, unsigned char*& p, bool sizeOnly)
{
	size_t sz = s.length ();
	serialise (sz, p, sizeOnly);	
	copyTo (p, (const unsigned char*) s.c_str (), sz, sizeOnly);
}

void Serialise::serialise (Chunk::Ptr chunk, unsigned char*& p, bool sizeOnly)
{
	size_t sz = sizeof (*chunk);
	copyTo (p, (const unsigned char*) chunk.get_raw (), sz, sizeOnly);
}

void Serialise::serialise (BootTableEntry::Ptr bte, unsigned char*& p, bool sizeOnly)
{
	if (bte.isNull ()) bte = mdf::make_shared (new BootTableEntry ());

	serialise ((unsigned long) bte->empty, p, sizeOnly);
	serialise (bte->shortName, p, sizeOnly);
	serialise ((unsigned long) 0, p, sizeOnly);
	serialise ((unsigned long) bte->length, p, sizeOnly);
	serialise ((unsigned long) bte->loadAddress, p, sizeOnly);
	serialise ((unsigned long) bte->startAddress, p, sizeOnly);
	serialise ((unsigned long) bte->startBlock, p, sizeOnly);
}

void Serialise::serialise (FileEntry::Ptr file, unsigned char*& p, bool sizeOnly)
{
	serialise (file->name (), p, sizeOnly);
	serialise (file->size (), p, sizeOnly);
	serialise (file->chunks (), p, sizeOnly);
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

	delete buf;
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

	unsigned long startAddress;
	deserialise (startAddress, p);
	bte->startAddress = startAddress;

	unsigned long startBlock;
	deserialise (startBlock, p);
	bte->startBlock = startBlock;
}

}
