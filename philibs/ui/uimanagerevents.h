/////////////////////////////////////////////////////////////////////
//
//  
//
/////////////////////////////////////////////////////////////////////

#ifndef uimanagerevents_h
#define uimanagerevents_h

/////////////////////////////////////////////////////////////////////

#include "uieventobserver.h"
#include "uimanager.h"

/////////////////////////////////////////////////////////////////////

namespace ui {

/////////////////////////////////////////////////////////////////////

struct flickObserver :
  public eventObserver 
{
  flickObserver ( manager* pManager ) :
    mManager ( pManager ),
    mThreshold ( 1.0f ),
    mTimeout ( 0.125f ),
    mLastTime ( 0.0f ),
    mXlast ( 0.0f ),
    mYlast ( 0.0f ),
    mXvel ( 0.0f ),
    mYvel ( 0.0f)
      {}

  virtual void onMouseDown ( mouseEvent const& event )
      {
        mXlast = event.mMousePointers[ 0 ].mXpos;
        mYlast = event.mMousePointers[ 0 ].mYpos;
        
        // Make sure we don't get an accidental flick with no drag by
        // resetting mLastTime.
        mLastTime = 0.0f;
      }
  
  void updateVel ( mouseEvent const& event )
      {
        float xvel = event.mMousePointers[ 0 ].mXpos - mXlast;
        float yvel = event.mMousePointers[ 0 ].mYpos - mYlast;

        const float alpha = 0.1f;
        mXvel = mXvel * ( 1.0f - alpha ) + xvel * alpha;
        mYvel = mYvel * ( 1.0f - alpha ) + yvel * alpha;

        mXlast = event.mMousePointers[ 0 ].mXpos;
        mYlast = event.mMousePointers[ 0 ].mYpos;
      }

  virtual void onMouseDrag ( mouseEvent const& event )
      {
        updateVel ( event );
        
        mLastTime = event.getTimeStamp ();
      }
      
  virtual void onMouseUp ( mouseEvent const& event )
      {
        // If it's been too long since the last drag, than this isn't
        // a flick.
        if ( event.getTimeStamp () - mLastTime < mTimeout )
        {
          updateVel ( event );
          
          if ( fabsf ( mXvel ) > mThreshold || fabsf ( mYvel ) > mThreshold )
            doFlick ( event );
        }
        else
        {
          mLastTime = 0.0f;
        }
      }
  
  void doFlick ( mouseEvent const& event )
      {
        flickEvent tmpEvent;
        tmpEvent.set ( mXlast, mYlast, mXvel, mYvel );
        tmpEvent.setTimeStamp ( event.getTimeStamp () );
        tmpEvent.setBase ( event.getBase () );
        tmpEvent.setManager ( event.getManager () );
        
        mManager->onFlick ( tmpEvent );
      }

  manager* mManager;
  float mThreshold;
  float mTimeout;
  float mLastTime;
  float mXlast;
  float mYlast;
  float mXvel;
  float mYvel;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

struct clickObserver :
  public eventObserver
{
    // Helper class to store click info in time.
  struct click
  {
    click () :
      mDownTime ( -1.0f ),
      mUpTime ( -1.0f ),
      mXpos ( 0.0f ),
      mYpos ( 0.0f )
        {}
        
    bool isValid () const { return mDownTime >= 0.0f && mUpTime >= 0.0f; }
    void reset () { mDownTime = -1.0f; mUpTime = -1.0f; }
    
    void setDown ( mouseEvent const& event )
        {
          mXpos = event.mMousePointers[ 0 ].mXpos;
          mYpos = event.mMousePointers[ 0 ].mYpos;
          mDownTime = event.getTimeStamp ();
        }
        
    bool isInside ( mouseEvent const& event, float rangeThreshold )
        {
          mousePointer const& mouse = event.mMousePointers[ 0 ];
          
          return ( fabsf ( mouse.mXpos - mXpos ) < rangeThreshold
            && fabsf ( mouse.mYpos - mYpos ) < rangeThreshold );
        }
        
    bool isInside ( click const& clk, float rangeThreshold )
        {
          return ( fabsf ( clk.mXpos - mXpos ) < rangeThreshold
            && fabsf ( clk.mYpos - mYpos ) < rangeThreshold );
        }
    
    void setDrag ( mouseEvent const& event, float rangeThreshold )
        {
          if ( ! isInside ( event, rangeThreshold ) )
            reset ();
        }
        
    void setUp ( mouseEvent const& event, float rangeThreshold )
        {
          if ( mDownTime >= 0.0f && isInside ( event, rangeThreshold ) )
          {
            mUpTime = event.getTimeStamp ();
          }
          else
            reset ();
        }
        
    float calcDelta () const { return mUpTime - mDownTime; }
        
    float mDownTime;
    float mUpTime;
    float mXpos;
    float mYpos;
  };

    // Constructor.
  clickObserver ( manager* pManager ) :
    mManager ( pManager ),
    mSingleThreshold ( 0.125f ),
    mDoubleThreshold ( 0.5f ),
    mRangeThreshold ( 10.0f ),
    mHoldThreshold ( 1.5f )
      {}

    virtual void onMouseDown ( mouseEvent const& event )
        {
          if ( ! mSingle.isValid () )
          {
            mSingle.setDown ( event );
            mSingleEvent = event;
          }
          else
            mDouble.setDown ( event );
            
          mHold.setDown ( event );
        }
    
    virtual void onMouseDrag ( mouseEvent const& event )
        {
          if ( ! mSingle.isValid () )
            mSingle.setDrag ( event, mRangeThreshold );
          else
            mDouble.setDrag ( event, mRangeThreshold );
            
          mHold.setDrag ( event, mRangeThreshold );
        }
        
    virtual void onMouseUp ( mouseEvent const& event )
        {
          if ( ! mSingle.isValid () )
            mSingle.setUp ( event, mRangeThreshold );
          else
            mDouble.setUp ( event, mRangeThreshold );
          
          mHold.reset ();
          
          if ( mSingle.isValid () && ! mDouble.isValid () )
          {
            if ( mSingle.calcDelta () < mSingleThreshold )
            {
//PNIDBG
              mouseEvent tmpEvent ( event );
              tmpEvent.mMousePointers[ 0 ].mType = mousePointer::SingleClick;
              
              mManager->onSingleClick ( tmpEvent );
            }
            else
              mSingle.reset ();
          }
          else if ( mSingle.isValid () && mDouble.isValid () )
          {
              // Mult by 2.0f for isInside to allow second click
              // to stray just a little more.
            if ( mDouble.calcDelta () < mSingleThreshold &&
                mDouble.isInside ( mSingle, mRangeThreshold * 2.0f ) )
            {
//PNIDBG
              mouseEvent tmpEvent ( event );
              tmpEvent.mMousePointers[ 0 ].mType = mousePointer::DoubleClick;
              
              mManager->onDoubleClick ( tmpEvent );
            }

              // No matter what, if we have a second up event
              // we know we can wipe both the single and double
              // snapshots.
            mSingle.reset ();
            mDouble.reset ();
          }
        }
        
    //virtual void onSingleClick ( mouseEvent const& event );
    //virtual void onDoubleClick ( mouseEvent const& event );
    //virtual void onSingleOnlyClick ( mouseEvent const& event );
       
    //virtual void onAccelEvent ( accelEvent const& event ) {}
    //virtual void onKeyEvent ( keyEvent const& event ) {}
    //virtual void onSceneEvent ( sceneEvent const& event ) {}
    
    virtual void onHeartbeat ( heartbeatEvent const& event )
        {
            // Check if we've gone over the double click threshold.
            // If so, send a singleOnly event and clear things out.
          if ( mSingle.isValid  () &&
            event.getTimeStamp () - mSingle.mDownTime > mDoubleThreshold )
          {
//PNIDBG
            mouseEvent tmpEvent ( mSingleEvent );
            tmpEvent.mMousePointers[ 0 ].mType = mousePointer::SingleClickOnly;
            mManager->onSingleOnlyClick ( tmpEvent );
            mSingle.reset ();
            mDouble.reset ();
            mSingleEvent = ui::mouseEvent ();
          }
            
            // TODO: Fit hold event into this class rather
            // than having a separate class... there would
            // be lots of redundancies.  Current issue is
            // that the mSingle member gets reset after
            // (typically) only .5 seconds, but a hold only
            // pops after 1.5 seconds.  We'll need to keep
            // duplicate state for holds probably.
          if ( mHold.mDownTime >= 0.0f 
              && event.getTimeStamp () - mHold.mDownTime >= mHoldThreshold )
          {
//PNIDBG
            mouseEvent tmpEvent ( mSingleEvent );
            tmpEvent.mMousePointers[ 0 ].mType = mousePointer::Hold;
            mManager->onMouseHold ( tmpEvent );
            mHold.reset ();
            mSingleEvent = ui::mouseEvent ();
          }

        }
        
    //virtual void onFlick ( flickEvent const& event ) {}

      
  manager* mManager;
  mouseEvent mSingleEvent;
  float mSingleThreshold;
  float mDoubleThreshold;
  float mRangeThreshold;
  float mHoldThreshold;
  click mSingle;
  click mDouble;
  click mHold;

};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end namespace ui

/////////////////////////////////////////////////////////////////////

#endif // uimanagerevents_h

