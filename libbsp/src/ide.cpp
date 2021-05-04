#include "../include/ide.h"
#include <stdio.h>

ide::ide (unsigned int base_address)
	:_rm ((ide_register_map*) base_address)
{
	printf ("ide::ide %x\n\r",base_address);	
}


char ide::read_status() const
{
	printf ("ide::read_status\n\r");
	printf ("%x",&_rm->command);	
	return _rm->command;	
}

void ide::ident (short* buf)
{
	printf ("ide::ident\n\r");
	send_command (_ident);
	wait_ready ();
	for (int i = 0; i < 256; i++)
		*buf++ = _rm->data;
}

void ide::send_command (char command)
{
	printf ("ide::send_command\n\r");
	wait_ready ();
	_rm->command = command;
}

void ide::wait_ready () const
{
	printf ("ide::wait_ready\n\r");
	while ((read_status() & _ready) == 0)
		;
}
