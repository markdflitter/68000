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

string CommandReader::read ()
{
	char buf[255];

	char* p = buf;
	for (;;)
	{
 		char c = getchar ();
		if (c == '\r')
			break;		
 		else if (c == 127) //backspace
		{
			if (p > buf) p--;
			putchar (c);
		}
		else if (c == 27)
		{
 			char c1 = getchar ();
			if (c1 == '[')
			{
 				char c2 = getchar ();
				if (c2 == 'A')
				{
 					if (m_pos > 0) m_pos--;
					string s = getHistoryItem (m_pos);
					if (s.length () > 0) 
					{
						while (p > buf)
						{
							putchar (127);
							--p;
						}

						memcpy (p, s.c_str (), s.length());
						p += s.length ();
					
						for (char* p2 = buf; p2 < p; p2++)
							putchar (*p2);
					}
				}
				else if (c2 == 'B')
				{
					if (m_pos < m_history.size() - 1) m_pos++;
 					string s = getHistoryItem (m_pos);
					if (s.length () > 0) 
					{
						while (p > buf)
						{
							putchar (127);
							--p;
						}

						memcpy (p, s.c_str (), s.length());
						p += s.length ();
					
						for (char* p2 = buf; p2 < p; p2++)
							putchar (*p2);
					}
				}
			}
		}
		else
		{
 			putchar (c);
			*p++ = c;
		}
	}
 
	*p = '\0';

	string result (buf);
	addHistoryItem (result);

	return result;
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

void CommandReader::addHistoryItem (const std::string& item)
{
	if (item.length () == 0) return ;

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
}

}
