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
	printf ("0x%x\n\r",&(_rm->command));	

	unsigned char result =  _rm->command;	

	printf ("0x%x\n\r",result);

	return result;
}

void ide::ident (char* buf)
{
	//wait_ready ();

	//printf ("device select address 0x%x\n\r",&(_rm->device_select));
	unsigned char device_select = _rm->device_select;
	printf ("device_select 0x%x\n\r",device_select);
	_rm->device_select = 0xE0;
	for (int i =  0; i < 10; i++)
		printf ("wait...\n\r");
	device_select = _rm->device_select;
	printf ("device_select 0x%x\n\r",device_select);

	unsigned char status = _rm->command;
	printf ("status 0x%x\n\r", status);


	return ;
	printf ("ide::ident\n\r");
	send_command (0xEC);
	printf ("sent command\n\r");

	wait_ready ();
	printf ("wait for DRQ\n\r");

	while ((read_status() & 0x8) == 0)
		printf ("dum-de-dum\n\r");
	
	printf ("got ident\n\r");
	for (int i = 0; i < 256; i++)
	{
		printf (".");	
	    short v = _rm->data;
		printf ("%x",v);
		*buf++ = (char) v;
		v = v >> 8;
		*buf++ = (char) v;
	}
}

void ide::send_command (char command)
{
	printf ("ide::send_command\n\r");
	wait_ready ();
	printf ("sending command\n\r");
	_rm->command = command;
}

void ide::wait_ready () const
{
	printf ("ide::wait_ready\n\r");
	while ((read_status() & _ready) == 0)
		printf ("not ready\n\r");
	while ((read_status() & 0x80) == 1)
		printf ("busy\n\r");
}
