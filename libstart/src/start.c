#include "../include/start.h"

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
	clear_bss();
  	call_static_constructors ();
	return main ();
}


