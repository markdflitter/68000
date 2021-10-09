#include "Serialise.h"
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

}
