#include <stdio.h>
#include <string.h>


class iAnimal 
{
public:
 	virtual void noise () = 0;
};

class Cat : public iAnimal
{
public:
	virtual void noise () {printf ("%s\n\r", "miaow");}
};

class Dog : public iAnimal
{
public:
	virtual void noise () {printf ("%s\n\r", "woof");}
};

void printhelp (void)
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
}

int main ()
{
	iAnimal* a1 = new Cat ();
	a1->noise ();

	a1 = new Dog ();
	a1->noise ();


	const char* version = "Zebulon V1.2\n\r";

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
		if (strcmp (buf, "help") == 0) printhelp();
	}

	return 0;
}

