#ifndef FILER_H
#define FILER_H

#include "FAT.h"

namespace Zebulon
{

class Filer
{
public:
	Filer ();

	void load();
	int format (int diskSize);

	int fopen (const std::string& filename, const std::string& mode);

	void diag () const;
	FreeSpace getFreeSpace () const;
private:
	FAT m_FAT;
};

}

#endif

