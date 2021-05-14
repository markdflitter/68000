#include <stdio.h>
#include <string.h>
#include <ide.h>

void printhelp (void)
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
}

void test()
{
	ide i;
	i.test ();
}

void test1()
{
	ide i;
	i.test1 ();
}

void test2()
{
	ide i;
	i.test2 ();
}


int main ()
{
	const char* version = "Zebulon V1.9";
	printf ("%s\n\r",version);
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
		if (strcmp (buf, "version") == 0) printf ("%s",version);
		if (strcmp (buf, "help") == 0) printhelp ();
		if (strcmp (buf, "test") == 0) test ();
		if (strcmp (buf, "test1") == 0) test1 ();
		if (strcmp (buf, "test2") == 0) test2 ();
	}

	return 0;
}

