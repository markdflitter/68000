#ifndef TIMER_H
#define TIMER_H

#include <time.h>

namespace mdf
{

class timer
{
public:
	timer ();

	void reset ();
	
	clock_t elapsed () const;
private:
	clock_t m_startTime;
};

}

#endif
