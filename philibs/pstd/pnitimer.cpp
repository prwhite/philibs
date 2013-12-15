/////////////////////////////////////////////////////////////////////
//
//	file: pnitimer.cpp
//
/////////////////////////////////////////////////////////////////////

#include "pnitimer.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {
		
/////////////////////////////////////////////////////////////////////

// the big four are in the platform specific cpp file

/////////////////////////////////////////////////////////////////////

	// system timer API
pni::pstd::float64
getSystemTime()
{
	static timer systemTimer;
	return systemTimer.get();
}


// Get the elapsed time since the timer was last reset.
pni::pstd::float64
timer::
get () const
{
	return getSystemTime () - startTime + offset;
}



// Get the elapsed time since the timer was last reset.
pni::pstd::float64
timer::
getStartTime () const
{
	return startTime;
}



// Reset the timer.
void
timer::
reset ()
{
	startTime = getSystemTime ();
}


void
timer::
setOffset ( pni::pstd::float64 offsetIn )
{
	offset = offsetIn;
}

pni::pstd::float64
timer::
getOffset () const
{
	return offset;
}

/////////////////////////////////////////////////////////////////////


	} // end of namespace pstd
} // end of namespace pni 


