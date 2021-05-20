#include "../include/string"
#include <size_t.h>
#include <string.h>

namespace std
{

string::string (const char* s) : m_s (0)
{
	assign (s);
}

string::string (const string& other) : m_s (0)
{
	assign (other.c_str ());
}

string::~string ()
{
	delete m_s;
	m_s = 0;
}

string& string::operator= (const string& other)
{
	if (&other != this)
	{
		assign (other.c_str ());
	}

	return *this;
}

bool string::operator== (const string& other)
{
	return strcmp (m_s, other.c_str ()) == 0;
}

const char* string::c_str () const
{
	return m_s;
}

void string::assign (const char* s)
{
	if (m_s)
	{
		delete m_s;
		m_s = 0;
	}

	if (s)
	{
		size_t length = strlen (s);
		m_s = new char [length + 1];
		memcpy (m_s, s, length + 1);
	}
}

};

