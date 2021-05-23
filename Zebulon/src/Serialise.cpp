#include "Serialise.h"
#include <string.h>
#include <stdio.h>

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

unsigned char* Serialise::serialise (const std::string& s, unsigned char* p)
{
	size_t sz = s.length ();
	p = serialise (sz, p);	
	return copyTo (p, s.c_str (), sz);
}

unsigned char* Serialise::serialise (const SpaceManager::Chunk& chunk, unsigned char* p)
{
	size_t sz = sizeof (chunk);
	return copyTo (p, &chunk, sz);
}

unsigned char* Serialise::serialise (const FAT::File& file, unsigned char* p)
{
	p = serialise (file.name (), p);
	p = serialise (file.bytes (), p);
	p = serialise (file.chunks (), p);

	return p;
}

unsigned char* Serialise::deserialise (unsigned long& l, unsigned char* p)
{
	size_t sz = sizeof (l);
	return copyFrom (&l, p, sz); 
}

unsigned char* Serialise::deserialise (std::string& s, unsigned char* p, size_t maxLength)
{
	size_t sz = 0;
	p =	deserialise (sz, p);
	if (sz > maxLength) sz = maxLength;

	char* buf = new char [sz + 1];
	buf [sz] = '\0';

	p = copyFrom (buf, p, sz);
	
	s = std::string (buf);
				
	return p;
}

unsigned char* Serialise::deserialise (SpaceManager::Chunk& chunk, unsigned char* p)
{
	size_t sz = sizeof (chunk);
	return copyFrom (&chunk, p, sizeof (chunk));
}

unsigned char* Serialise::deserialise (FAT::File& file, unsigned char* p)
{
	std::string name;	
	p = deserialise (name, p, 20);
	file.setName (name);

	p = deserialise (file.bytes (), p);
	p = deserialise (file.chunks (), p);

	return p;
}
