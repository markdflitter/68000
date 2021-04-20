#include "../include/start.h"
#include "../include/libgloss_start.h"
#include <stdio.h>

extern int main (void);

extern char* __ctors_begin;
extern char* __ctors_end;

typedef void (*f) (void);
 
void call_static_constructors ()
{
	char** start = &__ctors_begin;

    char** end = &__ctors_end;

    while (start != end)
    {
        char* fp = *start;
        ((f) fp)();
        start += 1;
    }
}

extern "C" int __main (void)
{
	__init ();	
	printf ("%s", "bsp initialised\n\r");

	printf ("%s", "calling static constructors\n\r");
  	call_static_constructors ();

	printf ("%s", "handing off to main()\n\r");
	return main ();
}


