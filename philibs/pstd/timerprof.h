/////////////////////////////////////////////////////////////////////
//
//	class: timerProf
//
/////////////////////////////////////////////////////////////////////

#ifndef pnitimerprof_h
#define pnitimerprof_h

/////////////////////////////////////////////////////////////////////

// #include "pnipstddef.h"
// #include "pnipstd.h"

#include "pnitimer.h"
#include <string>
#include <iostream>

/////////////////////////////////////////////////////////////////////

#ifndef PSTDPROFDISABLE
  #define PSTDPROFCALL(call) call
#else
  #define PSTDPROFCALL(call)
#endif

//#define PROFQUIET

/////////////////////////////////////////////////////////////////////

namespace pstd {

std::ostream& getTpStr ();
		
/////////////////////////////////////////////////////////////////////

class timerProf 
{
	public:

		// The timer is reset upon default construction
		timerProf ( std::string const& msg, unsigned int countMod = 120 ) :
			mMsg ( msg ),
			mCount ( 0 ),
			mCountMod ( countMod ),
			mStartTime ( -1.0 ),
			mTotalTime ( 0.0 )
				{
					
				}
		~timerProf ()
				{

				}

		// Reset the timer.
		void reset () {}


		void setMessage ( std::string const& msg ) { mMsg = msg; }

		void start ()
				{
#ifndef PROFQUIET
					mStartTime = mTimer.get ();
#endif // QUIET
				}

		void stop ()
				{
#ifndef PROFQUIET
					if ( mStartTime < 0.0 )
						return;

					pni::pstd::float64 mDelta = mTimer.get () - mStartTime;
					mTotalTime += mDelta;
					++mCount;

					if ( mCount == mCountMod )
					{
						getTpStr () << mMsg << " (avg " << mCount << ") : " << mTotalTime / mCount << std::endl;
//             getTpStr ().flush ();
						mTotalTime = 0.0;
						mCount = 0;
					}
#endif // QUIET
				}


	protected:
		
	private:
		pni::pstd::timer mTimer;
		pni::pstd::float64 mStartTime;
		pni::pstd::float64 mTotalTime;
		std::string mMsg;
		unsigned int mCount;
		unsigned int mCountMod;
};

/////////////////////////////////////////////////////////////////////

} // end of namespace pstd

#endif // pnitimer_h


