#ifndef SHELL_H
#define SHELL_H

#include "FAT.h"

class Shell {
public:
	Shell (FAT& fat, unsigned int& time);
	void run () const;
private:
	FAT& m_fat;
	unsigned int& m_time;
};

#endif
