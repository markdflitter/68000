#include "../include/start.h"
#include "../include/libgloss_start.h"


extern int main (void);

void call_static_constructors ()
{
}

int __main (void)
{
  	call_static_constructors ();

	__init ();

	return main ();
}


