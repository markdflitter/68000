#include <stdio.h>

extern char* __ctors_begin;
extern char* __ctors_end;


class C {
public:
  C (const char* s) : _s (s) {printf ("called\n\r");}
  void print () {printf ("%s\n\r",_s);}
private:
  const char* _s;
};

C c ("hello");
C c1 ("bye");


typedef void (*f) (void);

void call_constructors ()
{
	char** start = &__ctors_begin;
	printf ("%x\n\r",start);

	char** end = &__ctors_end;
	printf ("%x\n\r",end);

	while (start != end)
	{
		char* fp = *start;
		printf ("%x\n\r",fp);
		((f) fp)();
		start += 1;
	}
	printf ("return\n\r");
}


int main ()
{
	C* d = new C ("hi");

	call_constructors ();
	printf ("back\n\r");

	c.print ();
	c1.print ();

	return 0;
}

