#include <stdio.h>
#include <string.h>
#include <MC68230.h>

void printhelp (void)
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
	printf ("on\t - turn LED on\n\r");
	printf ("off\t - turn LED off\n\r");
}

void on ()
{
	MC68230 PIT;
	PIT.set_port_c_direction (0xFF);
	PIT.write_port_c (0xFF);
}

void off ()
{
	MC68230 PIT;
	PIT.set_port_c_direction (0xFF);
	PIT.write_port_c (0x0);
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
		if (strcmp (buf, "on") == 0) on ();
		if (strcmp (buf, "off") == 0) off ();
	}

	return 0;
}

