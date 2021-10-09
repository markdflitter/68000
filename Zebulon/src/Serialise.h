#ifndef SERIALISE_H
#define SERIALISE_H

#include <string>
#include "Chunk.h"
#include <list>
#include "FileEntry.h"

namespace Zebulon
{

class Serialise
{
public:
	static void serialise (unsigned long l, unsigned char*& p);
	static void serialise (const std::string& s, unsigned char*& p);
	static void serialise (Chunk::Ptr chunk, unsigned char*& p);
	static void serialise (FileEntry::Ptr file, unsigned char*& p);
	template <class T> static void serialise (const std::list<T>& list, unsigned char*& p)
	{
		size_t num = list.size ();
		serialise (num, p);

		for (typename std::list<T>::const_iterator i = list.begin (); i != list.end (); i++)
				serialise (*i, p);
	}

	static void deserialise (unsigned long& l, const unsigned char*& p);
	static void deserialise (std::string& s, const unsigned char*& p, size_t maxLength);
	static void deserialise (Chunk::Ptr chunk, const unsigned char*& p);
	static void deserialise (FileEntry::Ptr file, const unsigned char*& p);
	template <class T> static void deserialise (std::list <T>& list, const unsigned char*& p)
	{
		size_t num = 0;
		deserialise (num, p);

		for (int i = 0; i < num; i++)
		{
			T t;
			deserialise (t, p);
			list.push_back (t);
		}
	}

	template <class T> static void deserialise (std::list <mdf::shared_ptr <T> >& list, const unsigned char*& p)
	{
		size_t num = 0;
		deserialise (num, p);

		for (int i = 0; i < num; i++)
		{
			mdf::shared_ptr <T> t = mdf::make_shared (new T);
			deserialise (t, p);
			list.push_back (t);
		}
	}
	
};

}	
#endif

