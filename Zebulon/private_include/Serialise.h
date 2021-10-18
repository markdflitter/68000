#ifndef SERIALISE_H
#define SERIALISE_H

#include <string>
#include "Chunk.h"
#include <list>
#include <vector>
#include "FileEntry.h"
#include "BootTableEntry.h"

namespace Zebulon
{

class Serialise
{
public:
	static void serialise (unsigned long l, unsigned char*& p);
	static void serialise (const std::string& s, unsigned char*& p);
	static void serialise (Chunk::Ptr chunk, unsigned char*& p);
	static void serialise (FileEntry::Ptr file, unsigned char*& p);
	static void serialise (BootTableEntry::Ptr bte, unsigned char*& p);
	template <class T> static void serialise (const std::list<T>& list, unsigned char*& p)
	{
		size_t num = list.size ();
		serialise (num, p);

		for (typename std::list<T>::const_iterator i = list.begin (); i != list.end (); i++)
				serialise (*i, p);
	}

	template <class T> static void serialise (const std::vector<T>& v, unsigned char*& p)
	{
		size_t num = v.capacity ();
		serialise (num, p);

		unsigned char* start = p;
		for (size_t i = 0; i < num; i++)
		{
			serialise (v [i], p);
		}
	}

	static void serialise (const std::vector<BootTableEntry::Ptr>& v, unsigned char*& p)
	{
		size_t num = v.capacity ();
		for (size_t i = 0; i < 8; i++)
			serialise (num, p);

		unsigned char* start = p;
		for (size_t i = 0; i < num; i++)
		{
			serialise (v [i], p);
		}
	}

	static void deserialise (unsigned long& l, const unsigned char*& p);
	static void deserialise (std::string& s, const unsigned char*& p, size_t maxLength);
	static void deserialise (Chunk::Ptr chunk, const unsigned char*& p);
	static void deserialise (FileEntry::Ptr file, const unsigned char*& p);
	static void deserialise (BootTableEntry::Ptr bte, const unsigned char*& p);
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
	template <class T> static void deserialise (std::vector <T>& v, const unsigned char*& p)
	{
		size_t num = 0;
		deserialise (num, p);

		for (int i = 0; i < num; i++)
		{
			T t;
			deserialise (t, p);
			v [i] = t;
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
	
	static void deserialise (std::vector <BootTableEntry::Ptr>& v, const unsigned char*& p)
	{
		size_t num = 0;
		for (size_t i = 0; i < 8; i++)
			deserialise (num, p);

		for (int i = 0; i < num; i++)
		{
			BootTableEntry::Ptr t = mdf::make_shared (new BootTableEntry ());
			deserialise (t, p);
			v [i] = t;
		}
	}
};

}	
#endif

