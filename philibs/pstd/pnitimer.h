/////////////////////////////////////////////////////////////////////
//
//	class: timer
//
/////////////////////////////////////////////////////////////////////

#ifndef pnitimer_h
#define pnitimer_h

/////////////////////////////////////////////////////////////////////

// #include "pnipstddef.h"
// #include "pnipstd.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {
		
typedef double float64;

float64 getSystemTime ();

/////////////////////////////////////////////////////////////////////

class timer 
{
	public:

		// The timer is reset upon default construction
		timer ();
		timer ( const timer& orig );
		virtual ~timer ();
		timer& operator = ( const timer& orig );

		// Get the elapsed time since the timer was last reset.
		pni::pstd::float64 get () const;

		// Reset the timer.
		void reset ();

		// Timer offset will bias getTime by amount set
		void setOffset ( pni::pstd::float64 offsetIn );
		pni::pstd::float64 getOffset () const;

	protected:
		
	private:
		pni::pstd::float64 startTime;
		pni::pstd::float64 offset;
		pni::pstd::float64 frequency;	// implementation can decide to use this or not

		// Get the current time.
		pni::pstd::float64 getSystemTime () const;

		// Get the start time.
		pni::pstd::float64 getStartTime () const;

};

/////////////////////////////////////////////////////////////////////

	} // end of namespace util 
} // end of namespace pni 

#endif // pnitimer_h


