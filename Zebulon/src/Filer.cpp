#include "Filer.h"
#include <stdio.h>

namespace Zebulon
{

Filer::Filer ()
{
	printf ("Filer::Filer()\n\r");
}


void Filer::load ()
{
	m_FAT.load ();
}


int Filer::format (int diskSize)
{
  	return m_FAT.initialise (diskSize);
}

int Filer::fopen (const std::string& filename, const std::string& mode)
{
	printf ("Filer::fopen %s %s\n\r", filename.c_str (), mode.c_str ());
	return -1;
}

void Filer::diag () const
{
	m_FAT.diag ();
}

FreeSpace Filer::getFreeSpace () const
{
	return m_FAT.getFreeSpace ();
}

}
