#include "../private_include/CommandReader.h"
#include "../private_include/CommandReader.h"
#include "../private_include/CommandReader.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include "../private_include/Utils.h"
#include "../private_include/version.h"

using namespace std;

namespace 
{

string getStr (FILE* f)
{
	vector<char> v;
	v.reserve (255);

	char ch;
	while (!feof (f) && (fread (&ch, 1, 1, f) == 1) && (ch != '\n'))
		v.push_back (ch);

	// skip the following /r
	fread (&ch, 1, 1, f);

	v [v.size ()] = '\0';	

	return string (v.data ());
}

}
namespace Zebulon
{

#define BACKSPACE 127
#define ESCAPE 27
#define MAX_HISTORY 20
#define FILENAME ".history.txt"

CommandReader::CommandReader () : m_pos (0)
{
	m_p = m_buf;
}

void CommandReader::loadHistory ()
{
	clearHistory ();
	FILE* f = fopen (FILENAME, "rb");
	if (f == 0) 
	{
		printf (">>> failed to open history file\n\r");
		return ;
	}

	string readIdent = getStr (f);
	if (readIdent != HISTORY_IDENT)
	{
		printf (">>> HISTORY - ident mismatch.  Expected %s, got %s\n\r", HISTORY_IDENT, readIdent.c_str ());
		return ;
	}

	string readVersion = getStr (f);
	if (readVersion != HISTORY_VERSION)
	{
		printf (">>> HISTORY - version mismatch.  Expected %s, got %s\n\r", HISTORY_VERSION, readVersion.c_str ());
		return ;
	}

	while (!feof (f))
	{
		string item = getStr (f);
		m_history.push_back (item);
		m_pos = m_history.size ();
	}

	printf ("loaded command history: %d item(s)\n\r", m_history.size ());

	fclose (f);
}

string CommandReader::read ()
{
	for (;;)
	{
 		char c = getchar ();
		if (c == '\r') {*m_p = '\0'; break; }
 		else if (c == BACKSPACE) { delChar (); continue ; }
		else if (c == ESCAPE) {
 			while (m_p > m_buf) delChar ();

			while ((c = getchar ()) == ESCAPE) m_pos = m_history.size ();

			if (c == '[') {
 				switch (getchar ()) {
					case 'A' : if (m_pos > 0) m_pos--; break;
					case 'B' : if (m_pos < m_history.size() - 1) m_pos++; break;
					default: continue;
				}
			
				loadHistoryItem (m_pos);

				continue ;
			}
		}

 		putchar (c);
		*m_p++ = c;
	}
	m_p = m_buf;
	printf ("\n\r");

	return string (m_buf);
}

string CommandReader::addHistoryItem (const std::string& item)
{
	if (item.length () == 0) return item;
	vector<string> tokens = Utils::tokenize (item);
	if (tokens.size () > 1 && tokens [0] == "history" && tokens [1] != "show" && tokens [1] != "clear")
		return item;

	for (list<string>::iterator i = m_history.begin (); i != m_history.end (); i++)
	{
		if ((*i) == item)
		{
			i = m_history.erase (i);
			break;
		}
	}

	while (m_history.size () >= MAX_HISTORY)
		m_history.pop_front ();

	m_history.push_back (item);
	m_pos = m_history.size ();

	saveHistory ();

	return item;
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
	m_pos = m_history.size ();
}

void CommandReader::saveHistory () const
{
	FILE* f = fopen (FILENAME, "wb");
	if (f == 0) 
	{
		printf (">>> failed to open history file\n\r");
		return ;
	}

	{
		char buffer [255];
		string identversion = string (HISTORY_IDENT) + string ("\n\r") + string (HISTORY_VERSION) + string ("\n\r");
		sprintf (buffer, "%s\n\r", identversion.c_str ());
		fwrite (buffer, 1, identversion.length (), f);
	}

	for (list<string>::const_iterator i = m_history.begin (); i != m_history.end (); i++)
	{
		char buffer [255];
		sprintf (buffer, "%s\n\r", (*i).c_str ());
		fwrite (buffer, 1, (*i).length () + 2, f);
	}

	fclose (f);
}


void CommandReader::loadHistoryItem (size_t item)
{
	string s = getHistoryItem (item);
	memcpy (m_buf, s.c_str (), s.length() + 1);
		
	printf ("%s", m_buf);	

	m_p = m_buf + s.length ();
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

void CommandReader::delChar ()
{
	if (m_p > m_buf)
	{
		m_p--;
		putchar (127);
	}
}

}	

