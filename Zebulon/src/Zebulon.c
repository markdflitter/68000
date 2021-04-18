#include <stdio.h>
#include <string.h>

void printhelp ()
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
}

void main (void)
{
	const char* version = "Zebulon V1.1\n\r";

	printf (version);
	printf ("type help for help\n\r");

	char buf [21];
	int exit = 0;
	while (!exit)
	{
		printf ("$ ");
		char* p = gets (buf);
		*p = '\0';
		printf ("\n\r");

		if (strcmp (buf, "exit") == 0) exit = 1;
		if (strcmp (buf, "version") == 0) printf (version);
		if (strcmp (buf, "help") == 0) printhelp();
	}
}

