#include <stdio.h>
#include <string.h>

void printhelp ()
{
	puts ("exit\t - exit to monitor\n\r");
	puts ("version\t - print version\n\r");
	puts ("help\t - print this help\n\r");
}

void main (void)
{
	char* version = "Zebulon V1.0\n\r";

	puts (version);
	puts ("type help for help\r\n");

	char buf [21];
	int length;
	int exit = 0;
	while (!exit)
	{
		length = 0;
		puts ("$ ");
		char* p = gets (buf);
		*p = '\0';
		puts ("\n\r");

		if (strcmp (buf, "exit")) exit = 1;
		if (strcmp (buf, "version")) puts (version);
		if (strcmp (buf, "help")) printhelp();
	}
}

