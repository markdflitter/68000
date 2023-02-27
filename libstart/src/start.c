#include "../include/start.h"
#include <bsp.h>
#include <stdio.h>

extern int main (void);

typedef void (*f) (void);

extern char __bss_begin[];
extern char __bss_end[];

void clear_bss ()
{
	for (char* p = &__bss_begin[0]; p != &__bss_end[0]; p++)
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
#ifndef APP
	asm ("moveb #6, 0xE00001");
#endif

	clear_bss();

#ifndef APP
	asm ("moveb #7, 0xE00001");
#endif

#ifndef APP
	__init ();
	asm ("moveb #8, 0xE00001");
	__putstr ("bsp initialised\n\r");
#endif

#ifndef APP
	__putstr ("calling static constructors\n\r\n\r");
#endif

  	call_static_constructors ();

#ifndef APP
	asm ("moveb #9, 0xE00001");
	__putstr ("handing off to main()\n\r");
#endif

	return main ();
}


