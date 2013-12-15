/////////////////////////////////////////////////////////////////////
//
//	file: pnitimer.cpp
//
/////////////////////////////////////////////////////////////////////

#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

#include "pnitimer.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {

timer::
timer () :
	offset ( 0.0 ),
	frequency ( 0.0 )
{
#if DID_SOMEONE_JUST_BLINDLY_ADD_THIS_CODE
	LARGE_INTEGER lfreq;
	if ( QueryPerformanceFrequency ( &lfreq ) )
		frequency = 1.0 / lfreq.QuadPart;
	else
#endif
		frequency = -1.0;
	reset ();
}



timer::
timer ( const timer& orig ) :
	startTime ( orig.startTime ),
	frequency ( orig.frequency )
{
}


timer::
~timer ()
{

}



timer&
timer::
operator = ( const timer& orig )
{
	startTime = orig.startTime;	
	frequency = orig.frequency;
	return *this;
}
		
/////////////////////////////////////////////////////////////////////

// Get the current time.
pni::pstd::float64
timer::
getSystemTime () const
{
    struct timeval timeVal;
    gettimeofday ( &timeVal, NULL );

    return timeVal.tv_sec + timeVal.tv_usec / 1000000.0;
}

/////////////////////////////////////////////////////////////////////


	} // end of namespace pstd
} // end of namespace pni 


