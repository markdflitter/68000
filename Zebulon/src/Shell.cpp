#include "Shell.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <time.h>

const char* version = "Z-Shell V2.00.0000";
const char* filename = "Zebulon_V2.0.0000";

using namespace std;

namespace {

vector<string> tokenize (const string& command)
{
	vector<string> result;

	const char* str = command.c_str ();
	char buf [255];
	char* cur = buf;
	while (*str != '\0')
	{
		if (*str != ' ')
			*cur++ = *str;
		else
		{
			*cur = '\0';
			result.push_back (string (buf));
			cur = buf;
		}
		str++;
	}

	if (cur != buf)
	{
		*cur = '\0';
		result.push_back (string (buf));
		cur = buf;
	}

	return result;
}
}	

namespace {

void printHelp (void)
{
	printf ("version\t\t\t\t - print version\n\r");
	printf ("help\t\t\t\t - print this help\n\r");
	printf ("exit\t\t\t\t - exit to monitor\n\r");
	printf ("uptime\t\t\t\t - print time since boot\n\r");
}

void uptime ()
{
	printf ("%d seconds\n\r", clock () / 1000);
}

}

void Shell::run () const
{
	printf ("\n\r%s\n\r",version);
	printf ("type help for help\n\r\n\r");

	char buf [255];
	int exit = 0;
	while (!exit)
	{
		printf ("$ ");
		char* p = gets (buf);
		*p = '\0';
		printf ("\n\r");
		string command (buf);
		vector<string> tokens = tokenize (command);

		if (tokens.size () > 0)
		{
			if (tokens [0] == "version") printf ("%s\n\r",version);
			if (tokens [0] == "help") printHelp ();
			if (tokens [0] == "exit") exit = 1;
			if (tokens [0] == "uptime") uptime ();
		}
	}

	return ;
}
