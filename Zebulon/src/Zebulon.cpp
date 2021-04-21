#include <stdio.h>
#include <string.h>

extern int __main (void);

class C {
public:
	C (const char* s) : _s (s) {printf ("%s","helloworld\n\r");}
	void print () {printf ("%s\n\r",_s);}
	void set (const char*s) {_s = s;}
 private:
 	const char* _s;
 };

static int i = 0;
static int j = 1;
  
C c1 ("helloMark");
C c2 ("byeMark");

void test ()
{
	static C c3 ("hiho");
	c3.print ();
	c3.set ("hohi");
}


void printhelp ()
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
}

int main ()
{
	const char* version = "Zebulon V1.2\n\r";

	printf ("%s",version);
	printf ("type help for help\n\r");

	c1.print ();
	c2.print ();

	printf ("%d\n\r%d\n\r",i,j);

	test ();
	test ();

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
		if (strcmp (buf, "help") == 0) printhelp();
	}

	return 0;
}

