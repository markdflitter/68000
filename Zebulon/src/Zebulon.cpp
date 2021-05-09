#include <stdio.h>
#include <string.h>
#include <ide.h>

void printhelp (void)
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
	printf ("ident\t - ident the disk\n\r");
}

void ident ()
{
	char buf [513];

	ide i;
	i.ident (buf);
//	buf [512] = '\0';
	
//	printf ("ident\n\r");
//	for (int i = 0; i < 512; i++)
//		printf("%c\n\r",buf[i]);
}


int main ()
{
	const char* version = "Zebulon V1.6\n\r";

	printf ("%s",version);
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
		if (strcmp (buf, "ident") == 0) ident ();
	}

	return 0;
}

