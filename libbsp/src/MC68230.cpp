#include "../include/MC68230.h"

MC68230::MC68230 (unsigned int base_address)
	: _rm ((register_map*) base_address)
{
}

void MC68230::set_port_c_direction (unsigned char directions)
{
	_rm->port_c_data_direction = directions;
}

void MC68230::write_port_c (unsigned char value)
{
	_rm->port_c_data = value;
}

