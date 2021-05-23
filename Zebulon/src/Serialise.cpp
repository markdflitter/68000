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

unsigned char* Serialise::serialise (File::Ptr file, unsigned char* p)
{
	p = serialise (file->name (), p);
	p = serialise (file->size (), p);
	p = serialise (file->chunks (), p);

	return p;
}

unsigned char* Serialise::serialise (Chunk::ConstPtr chunk, unsigned char* p)
{
	size_t sz = sizeof (*chunk);
	return copyTo (p, chunk.get_raw (), sz);
}

unsigned char* Serialise::serialise (File::ConstPtr file, unsigned char* p)
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

unsigned char* Serialise::deserialise (File::Ptr file, unsigned char* p)
{
	string name;	
	p = deserialise (name, p, 20);
	file->setName (name);

	file_address_t size;	
	p = deserialise (size, p);
	file->setSize (size);

	list<Chunk::Ptr> chunks;
	p = deserialise (chunks, p);
	file->setChunks (chunks);

	return p;
}
