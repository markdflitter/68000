#include "SpaceManager.h"

namespace Zebulon
{

void SpaceManager::load ()
{
	do_load ();
}

int SpaceManager::initialise (int initialChunks)
{
	do_save ();
	return initialChunks;
}


void SpaceManager::do_load ()
{
}

void SpaceManager::do_save () const
{
}

}

