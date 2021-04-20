#include "../include/start.h"
#include "../include/libgloss_start.h"
#include <stdio.h>

extern int main (void);

typedef void (*f) (void);

extern f __ctors_begin;
extern f __ctors_end;

void call_static_constructors ()
{
	for (f* cur = &__ctors_begin; cur != &__ctors_end; cur++)
		(*cur)();
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


