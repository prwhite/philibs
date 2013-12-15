/////////////////////////////////////////////////////////////////////
//
//	file: pnitimer.cpp
//
//  BEWARE... queryPerformanceCounter only works on pentiums and above.
//
/////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/timeb.h>

#include <windows.h>
#include <winbase.h>

#include "pnitimer.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {
		
/////////////////////////////////////////////////////////////////////

timer::
timer () :
	offset ( 0.0 ),
	frequency ( 0.0 )
{
	LARGE_INTEGER lfreq;
	if ( QueryPerformanceFrequency ( &lfreq ) )
		frequency = 1.0 / lfreq.QuadPart;
	else
		frequency = -1.0;

	reset ();
}



timer::
timer ( const timer& orig ) :
	startTime ( orig.startTime ),
	frequency ( orig.frequency ),
	offset( orig.offset )
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

pni::pstd::float64
timer::
getSystemTime () const
{
	LARGE_INTEGER ltime;

	QueryPerformanceCounter ( &ltime );
	return ltime.QuadPart * frequency;
}

/////////////////////////////////////////////////////////////////////


	} // end of namespace pstd
} // end of namespace pni 


