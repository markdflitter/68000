#include "FAT.h"

namespace Zebulon
{

void FAT::load ()
{
	m_spaceManager.load ();
	do_load ();
}

int FAT::initialise (int diskSize)
{
	int result = m_spaceManager.initialise (diskSize);
	do_save ();
	return result;
}

void FAT::do_load ()
{
}

void FAT::do_save () const
{
}

}

