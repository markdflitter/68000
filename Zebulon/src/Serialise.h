#ifndef SERIALISE_H
#define SERIALISE_H

#include <string>
#include "Chunk.h"
#include "FileHeader.h"
#include <list>
#include <stdio.h>

class Serialise
{
public:
	static unsigned char* serialise (unsigned long l, unsigned char* p);
	static unsigned char* serialise (const std::string& s, unsigned char* p);
	static unsigned char* serialise (Chunk::Ptr chunk, unsigned char* p);
	static unsigned char* serialise (FileHeader::Ptr file, unsigned char* p);
	static unsigned char* serialise (Chunk::ConstPtr chunk, unsigned char* p);
	static unsigned char* serialise (FileHeader::ConstPtr file, unsigned char* p);
		template <class T> static unsigned char* serialise (const std::list<T>& list, unsigned char* p)
	{
		size_t num = list.size ();
		p = (unsigned char*) serialise (num, p);

		for (typename std::list<T>::const_iterator i = list.begin (); i != list.end (); i++)
				p = serialise (*i, p);

		return p;
	}

	static unsigned char* deserialise (unsigned int& i, unsigned char* p);
	static unsigned char* deserialise (unsigned long& l, unsigned char* p);
	static unsigned char* deserialise (std::string& s, unsigned char* p, size_t maxLength);
	static unsigned char* deserialise (Chunk::Ptr chunk, unsigned char* p);
	static unsigned char* deserialise (FileHeader::Ptr file, unsigned char* p);
	template <class T> static unsigned char* deserialise (std::list <T>& list, unsigned char* p)
	{
		size_t num = 0;
		p = deserialise (num, p);

		for (int i = 0; i < num; i++)
		{
			T t;
			p = deserialise (t, p);
			list.push_back (t);
		}

		return p;
	}
	template <class T> static unsigned char* deserialise (std::list <mdf::shared_ptr <T> >& list, unsigned char* p)
	{
		size_t num = 0;
		p = deserialise (num, p);

		for (int i = 0; i < num; i++)
		{
			mdf::shared_ptr <T> t = mdf::make_shared (new T);
			p = deserialise (t, p);
			list.push_back (t);
		}

		return p;
	}
};	

#endif

