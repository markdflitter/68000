#ifndef SHELL_H
#define SHELL_H

#include "FAT.h"

class Shell {
public:
	Shell (FAT& fat);
	void run () const;
private:
	FAT& m_fat;
};

#endif
