#include "../private_include/CommandReader.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>

using namespace std;

namespace Zebulon
{

CommandReader::CommandReader () : m_pos (0)
{
	addHistoryItem ("uptime");
	addHistoryItem ("diag heap");
	addHistoryItem ("diag filer");
	addHistoryItem ("history show");
	addHistoryItem ("help");
}

#define BACKSPACE 127
#define ESCAPE 27

string CommandReader::read ()
{
	char buf[255];
	char* p = buf;

	for (;;)
	{
 		char c = getchar ();
		if (c == '\r') break;		
 		else if (c == BACKSPACE) { delChar (buf, p); continue ; }
		else if (c == ESCAPE)
		{
 			while (p > buf) delChar (buf, p);

			while ((c = getchar ()) == ESCAPE) m_pos = m_history.size ();

			if (c == '[')
			{
 				switch (getchar ())
				{
					case 'A' : if (m_pos > 0) m_pos--; break;
					case 'B' : if (m_pos < m_history.size() - 1) m_pos++; break;
					default: continue;
				}
			
				string s = getHistoryItem (m_pos);
				memcpy (p, s.c_str (), s.length());
				p += s.length ();
					
				for (char* p2 = buf; p2 < p; p2++) putchar (*p2);

				continue ;
			}
		}

 		putchar (c);
		*p++ = c;
	}
 
	*p = '\0';

	return addHistoryItem (buf);
}

void CommandReader::showHistory () const
{
	size_t index = 0;
	for (list<string>::const_iterator i = m_history.begin (); i != m_history.end (); i++)
	{
		printf ("[%d]\t%s\n\r", index, (*i).c_str ());
		index++;
	}
}

void CommandReader::clearHistory ()
{
	m_history.clear ();
}

string CommandReader::getHistoryItem (size_t index) const
{
	size_t count = 0;
	for (list<string>::const_iterator i = m_history.begin (); i != m_history.end (); i++)
	{
		if (count == index) return (*i);
		count++;
	}

	return "";
}

void CommandReader::delChar (char* buf, char*& p)
{
	if (p > buf)
	{
		p--;
		putchar (127);
	}
}

	
string CommandReader::addHistoryItem (const std::string& item)
{
	if (item.length () == 0) return item;

	for (list<string>::iterator i = m_history.begin (); i != m_history.end (); i++)
	{
		if ((*i) == item)
		{
			i = m_history.erase (i);
			break;
		}
	}

	m_history.push_back (item);
	m_pos = m_history.size ();

	return item;
}

}
