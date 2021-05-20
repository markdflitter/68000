#include "../private_include/MC68230.h"

MC68230::MC68230 (unsigned int baseAddress)
	: m_registers ((Registers*) baseAddress)
{
}
