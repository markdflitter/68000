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

extern "C" int __main (void)
{
	__init ();	
	printf ("%s", "bsp initialsed");

	printf ("%s", "calling static constructors");
  	call_static_constructors ();

	printf ("%s", "handing off to main()");
	return main ();
}


