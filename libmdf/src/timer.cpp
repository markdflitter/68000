#include "../include/timer.h"

namespace mdf
{

timer::timer () 
{
	reset ();
}

void timer::reset ()
{
	m_startTime = clock ();
}
	
clock_t timer::elapsed () const
{
	return clock () - m_startTime;
}

}
