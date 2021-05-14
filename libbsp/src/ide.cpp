#include "../include/ide.h"
#include <stdio.h>

ide::register_access::register_access (MC68230& controller, unsigned char reg)
	: _controller (controller), _reg (reg)
{
	_controller.set_port_c_direction (0xFF);
	reset ();
}
		
unsigned char ide::register_access::read ()
{
	setup ();
	pulse (eRead);
	reset ();
	return 0;
}

void ide::register_access::setup ()
{
	unsigned char v1 = eCS | eWrite;
	unsigned char v2 = _reg | eWrite;
	unsigned char* p = (unsigned char*) 0xA00019;

	*p = v1;
	*p = v2;

//	_controller.write_port_c (eCS | eWrite);
//	_controller.write_port_c (_reg | eWrite);
}

void ide::register_access::reset ()
{
	_controller.write_port_c (eCS | eRead | eWrite);
}

void ide::register_access::pulse (unsigned char pin)
{
	_controller.write_port_c (_reg | pin | eWrite);
	_controller.write_port_c (_reg | eWrite);
}

ide::ide (unsigned int base_address)
	: _controller (base_address)
{
}

void ide::test ()
{
	unsigned char ds = read_register (eDeviceSelect);
	printf ("device select = %x\n\r",ds);
}


unsigned char ide::read_register (unsigned char reg)
{
	return access_register (reg).read ();
}

ide::register_access ide::access_register (unsigned char reg)
{
	return register_access (_controller, reg);
}




