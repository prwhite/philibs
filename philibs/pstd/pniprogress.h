/////////////////////////////////////////////////////////////////////
//
//	class: progress
//
//
/////////////////////////////////////////////////////////////////////


#ifndef pniprogress_h
#define pniprogress_h

/////////////////////////////////////////////////////////////////////

#include "pnitimer.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {
	
	  class progress;

/////////////////////////////////////////////////////////////////////

class progressEvent
{
  public:
  
    progressEvent ( progress* subject ) :
      mSubject ( subject ),
      mTimeStamp ( 0.0 ),
      mPercent ( 0.0f )
        {}
        
    progressEvent ( progress* subject, float64 timeStamp, float percent ) :
      mSubject ( subject ),
      mTimeStamp ( timeStamp ),
      mPercent ( percent )
        {}
    
    progress* mSubject;
    float64 mTimeStamp;
    float mPercent;
};

class progressObserver
{
  public:
    virtual void onProgressStart ( progressEvent const& event ) {}
    virtual void onProgressUpdate ( progressEvent const& event ) {}
    virtual void onProgressEnd ( progressEvent const& event ) {}
};

/////////////////////////////////////////////////////////////////////

class progress
{
  public:
    progress () :
      mProgressObserver ( 0 )
        {}
  
      // TODO: Think about managing a list of observers instead.
    void setProgressObserver ( progressObserver* pObs ) { mProgressObserver = pObs; }
    progressObserver* getProgressObserver () const { return mProgressObserver; }
    
  protected:
    void emitProgressStart ( progressEvent const& event )
        {
          if ( mProgressObserver )
            mProgressObserver->onProgressStart ( event );
        }

    void emitProgressUpdate ( progressEvent const& event )
        {
          if ( mProgressObserver )
            mProgressObserver->onProgressUpdate ( event );
        }

    void emitProgressEnd ( progressEvent const& event )
        {
          if ( mProgressObserver )
            mProgressObserver->onProgressEnd ( event );
        }
  
    progressObserver* mProgressObserver;

};

/////////////////////////////////////////////////////////////////////

class progressProxy :
  public progress,
  public progressObserver
{
  public:
    virtual void onProgressStart ( progressEvent const& event )
        { emitProgressStart ( event ); }
    virtual void onProgressUpdate ( progressEvent const& event )
        { emitProgressUpdate ( event ); }
    virtual void onProgressEnd ( progressEvent const& event )
        { emitProgressEnd ( event ); }

};


/////////////////////////////////////////////////////////////////////

	} // end of namespace pstd
} // end of namespace pni 

#endif // pniprogress_h


