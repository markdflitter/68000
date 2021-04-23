#include <stdio.h>
#include <string.h>


/*
class C
{
public:
	C () : i (4) {printf ("ctor\n\r");} 
	static C& singleton () {return _singleton;}
	void print () {printf ("%d\n\r",i);}
private:
	static C _singleton;
	int i;
};

C C::_singleton;

class iAnimal 
{
public:
 	virtual void noise () = 0;
};

class Cat : public iAnimal
{
public:
	virtual void noise () {printf ("%s", "miaow");}
};

class Dog : public iAnimal
{
public:
	virtual void noise () {printf ("%s", "woof");}
};
*/

void printhelp (void)
{
	printf ("exit\t - exit to monitor\n\r");
	printf ("version\t - print version\n\r");
	printf ("help\t - print this help\n\r");
}

int main ()
{
	//C::singleton ().print ();

	//iAnimal* a1 = new Cat ();
	//a1->noise ();

	//a1 = new Dog ();
	//a1->noise ();

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

