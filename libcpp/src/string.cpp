#include "../include/string"
#include <size_t.h>
#include <string.h>

namespace std
{

string::string () : m_s (new char [1])
{
	m_s [0] = '\0';
}

string::string (const char* s) : m_s (0)
{
	assign (s);
}

string::string (const string& other) : m_s (0)
{
	assign (other.c_str ());
}

string::string (size_t n, char c)
  :  m_s (new char [n + 1])
{
  memset (m_s, c, n);
  m_s [n] = '\0';
}

	
string::~string ()
{
	delete m_s;
	m_s = 0;
}

size_t string::length () const
{
	return strlen (m_s);
}

void string::resize (size_t n, char c)
{
	size_t len = length ();
	if (len > n)
	{
		char* new_s = new char [n + 1];
		memcpy (m_s, new_s, n);
		new_s [n] = '\0';
	
		delete m_s;
	
		m_s = new_s;
	}
	else if (len < n)
	{
		string pad (n - len, c);
		(*this) = (*this) + pad;
	}
}

string& string::operator= (const string& other)
{
	if (&other != this)
	{
		assign (other.c_str ());
	}

	return *this;
}

bool string::operator== (const string& other) const
{
	return strcmp (m_s, other.c_str ()) == 0;
}

bool string::operator!= (const string& other) const
{
	return !(*this == other);
}

char& string::operator[] (size_t index)
{
	return m_s [index];
}

char string::operator[] (size_t index) const
{
	return m_s [index];
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

std::string operator+ (const std::string& lhs, const std::string& rhs)
{
	size_t l1 = lhs.length ();
	size_t l2 = rhs.length ();

	string result;
	delete result.m_s;
	result.m_s = new char [l1 + l2 + 1];

	memcpy (result.m_s, lhs.m_s, l1);
	memcpy (result.m_s + l1, rhs.m_s, l2);
	result.m_s [l1 + l2] = '\0';
 
	return result;
}


};


