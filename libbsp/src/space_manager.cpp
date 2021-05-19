#include "../include/space_manager.h"

space_manager::space_manager ()
 	: _free (0)
{
	load ();
}

space_manager::space_manager (unsigned int size)
 : _free (0)
{
	format (size);
}

space_manager::~space_manager ()
{
	if (_free)
	{
		delete _free;
		_free = 0;
	}
}


space_manager::chunk* space_manager::allocate (unsigned int size)
{
	chunk* allocation = 0;
	
	if (_free)
	{
		if (_free->_length >= size)
		{
			allocation = new chunk ();
			allocation->_start = _free->_start;
			allocation->_length = size;
			_free->_start += size;
			_free->_length -= size;
		
			save ();
		}
	}

	return allocation;
}	

void space_manager::print ()
{
}

void space_manager::format (unsigned int size)
{
	_free->_start = 2;
	_free->_length = size;

	save ();
}

void space_manager::save ()
{
}

void space_manager::load ()
{
}
