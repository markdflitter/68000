#include "../private_include/Display.h"

Display::Display (unsigned int baseAddress) :
	m_display ((unsigned char*) baseAddress)
{
}

void Display::setNumber (unsigned int num)
{
	*m_display = num & 0xF;
}

