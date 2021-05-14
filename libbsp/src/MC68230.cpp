#include "../include/MC68230.h"

MC68230::MC68230 (unsigned int base_address)
	: _rm ((register_map*) base_address)
{
}
