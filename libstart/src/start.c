#include "../include/start.h"
#include "../include/libgloss_start.h"
#include <stdio.h>

extern int main (void);

typedef void (*f) (void);

extern char __bss_begin;
extern char __bss_end;

void clear_bss ()
{
	for (char* p = &__bss_begin; p != &__bss_end; p++)
		*p = 0;
}

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

	printf ("%s", "clearing bss\n\r");
 	clear_bss();

	printf ("%s", "calling static constructors\n\r");
  	call_static_constructors ();

	printf ("%s", "handing off to main()\n\r");
	return main ();
}


