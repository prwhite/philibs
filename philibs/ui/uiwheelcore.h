/////////////////////////////////////////////////////////////////////
//
//  class: wheelCore
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuiwheelcore_h
#define uiuiwheelcore_h

/////////////////////////////////////////////////////////////////////

#include "pniavg.h"
#include "uicommon.h"
#include "uievents.h"

#include "pnimathfp.h"

#include <cassert>

/////////////////////////////////////////////////////////////////////

namespace ui {
    
/////////////////////////////////////////////////////////////////////

class wheelCore 
{
  public:
    typedef float ValType;
    typedef int IndType;
  
    wheelCore () :
      mGlideTracker ( 1.0f ),
      mVelAvg ( 4 ),
      mMouseTracker ( 0.0f )
        {}
    ~wheelCore ();
    //wheelCore ( wheelCore const& rhs );
    //wheelCore& operator= ( wheelCore const& rhs );
    //bool operator== ( wheelCore const& rhs ) const;

    virtual bool setCurVal ( float val )  // false if clamped.
        {
          PNIFPGOOD ( val );
        
          mMouseTracker.set ( val );
          mCurVal = mMouseTracker.get ();
          
          return ( val == mCurVal );
        }
        
    float getCurVal () const { return mCurVal; }
    
    bool setCurModVal ( float val )
        {
          PNIFPGOOD ( val );
          PNIFPGOOD ( mModVal );
        
          return setCurVal ( val / mModVal );
        }
        
    float getCurModVal () const
        {
          PNIFPGOOD ( getCurVal () );
          PNIFPGOOD ( mModVal );
        
          return getCurVal () * mModVal;
        }

    virtual void setCurSelection ( IndType which )
        {
          deactivateCurSelection ();

          mCurSelection = which;

          PNIFPGOOD ( mItemSize );

          mCurVal = which * mItemSize;
          
          PNIFPGOOD ( mCurVal );
          
          mGoodDrag = false;
          mGliding = false;
          mVel = 0.0f;
          mVelAvg.reset ();
          
          mMouseTracker.set ( mCurVal );
          
          activateCurSelection ();
        }
        
    IndType getCurSelection () const { return mCurSelection; }

    void setFreeRun () { mItemSize = 0.0f; }
    void setItemSize ( float val ) { mItemSize = val; }
    float getItemSize () const { return mItemSize; }

    void setMinMax ( float minVal, float maxVal )
        { mMouseTracker.setMinMax ( minVal, maxVal ); }
    void setModMinMax ( float minVal, float maxVal )
        { 
          PNIFPGOOD ( minVal );
          PNIFPGOOD ( maxVal );
          PNIFPGOOD ( mModVal );
          setMinMax ( minVal / mModVal, maxVal / mModVal ); 
        }
    void clearMinMax ()
        { mMouseTracker.clearMinMax (); }

    void setDuration ( TimeType duration ) { mGlideTracker.setDuration ( duration ); }
    
  protected:
    virtual void activateCurSelection ()
        {
          onSetDirty ( true );
        }
        
    virtual void deactivateCurSelection ()
        {
          onSetDirty ( true );
        }

    virtual void updateCurSelection ()
        {
          PNIFPGOOD ( mCurVal );
          PNIFPGOOD ( mItemSize );

          mCurSelection = ( IndType ) ( mCurVal / mItemSize );

//printf ( "mCS 1 = %d mCV = %f mIS = %f\n", mCurSelection.get (), mCurVal.get (), mItemSize.get () );

          //mCurSelection = ( IndType ) ( mCurVal / mItemSize + 0.5f );  // To get round-up.

//printf ( "mCS 1 = %d mCV = %f mIS = %f\n", mCurSelection.get (), mCurVal.get (), mItemSize.get () );

          activateCurSelection ();
        }

    float getPos ( ui::mouseEvent const& event )
        {
        #ifndef PNI_TARGET_WIN32
          if ( mMouseDimension.get () == 0 )
            return event.mMousePointers[ 0 ].mXpos;
          else
            return event.mMousePointers[ 0 ].mYpos;
        #else
          if ( mMouseDimension.get () == 0 )
            return event.mMousePointers[ 0 ].mYpos;
          else
            return 480.0f - event.mMousePointers[ 0 ].mXpos;
        #endif  
        }

    void onWheelDown ( mouseEvent const& event )
        {
          mGlideTracker.reset ();
          
          mGoodDrag = isGoodMouseDown ( event );
          
          if ( mGoodDrag )
          {
//printf ( "onWheelDown::good drag\n" );
            deactivateCurSelection ();
          
            onSetDirty ( true );
            cancelGlide ();
            mVelAvg.reset ();
            mMouseTracker.onDown ( getPos ( event ) );
            
            onDragStart ( this, wheelCore::event ( event.getTimeStamp () ) );
          }
        }
        
    void onWheelDrag ( mouseEvent const& event )
        {
          if ( mGoodDrag )
          {
//printf ( "onWheelDrag::good drag\n" );
            onSetDirty ( true );
            mMouseTracker.onDrag ( getPos ( event ) );
            float vel = mMouseTracker.getDelta ();
            
            PNIFPGOOD ( vel );
            
        //printf ( " delta = %f\n", vel );
            if ( mGlideTracker.mDt != 0.0f )
              vel /= mGlideTracker.mDt;

            PNIFPGOOD ( vel );

            mVelAvg.appendSample ( vel );

            mCurVal = mMouseTracker.get ();

            PNIFPGOOD ( mCurVal );

//printf ( " cur val = %f\n", mCurVal.get () );

            PNIFPGOOD ( event.getTimeStamp () );
            
            onWheelScroll ( this, wheelCore::event ( event.getTimeStamp () ) );

        //printf ( " curAvg = %f\n", mVelAvg.getCurAverage () );
          }
        }
        
    void onWheelUp ( mouseEvent const& event )
        {
          if ( mGoodDrag )
          {
//printf ( "onWheelUp::good drag\n" );
            onSetDirty ( true );
            mGoodDrag = false;
            mMouseTracker.onUp ( getPos ( event ) );
            mCurVal = mMouseTracker.get ();

            PNIFPGOOD ( mCurVal );

            startGlide ();
          }
        }
        
    void onWheelHeartbeat ( heartbeatEvent const& event )
        {
          mGlideTracker.update ( event.getTimeStamp () );
          updateDrag ();
          updateGlide ();
        }

    struct event
    {
      event ( TimeType tstamp ) : mTimeStamp ( tstamp ) {}
      
      TimeType mTimeStamp;
    };
    
    bool isGoodDrag () const { return mGoodDrag; }
    
    virtual void onDragStart ( wheelCore* pWheel, event const& wevent ) = 0;
    virtual void onSelectionChanged ( wheelCore* pWheel, event const& wevent ) = 0;
    virtual void onWheelScroll ( wheelCore* pWheel, event const& wevent ) = 0;
    virtual void onSetDirty ( bool val ) = 0;
    virtual bool isGoodMouseDown ( mouseEvent const& mevent ) = 0;

    pni::pstd::box< float > mVelDamp;
    pni::pstd::box< float > mVelMin;
    pni::pstd::box< float > mModVal;
    pni::pstd::box< IndType > mMouseDimension;


  private:
    void updateDrag ()
        {
          if ( mGoodDrag )
          {
//printf ( "updateDrag::good drag\n" );
            onSetDirty ( true );
              // Append a 0 sample because drag is not called every frame.
              // This will result in a velocity a little lower than the actual
              // velocity.  If this sucks too much we'll try it a different way.
            mVelAvg.appendSample ( 0.0f );
          }
        }

    void startGlide ()
        {
          if ( mGliding )
            return;

          mGliding = true;
          
          mVel = mVelAvg.getCurAverage ();

          PNIFPGOOD ( mVel );
          
          if ( pni::math::Trait::abs ( mVel ) <= mVelMin )
            mVel = calcStaticVel ();
          
          PNIFPGOOD ( mVel );
        }

    void updateGlide ()
        {
          if ( ! mGliding )
            return;
            
          onSetDirty ( true );

          float mLastCurVal = mCurVal;
          float goal = calcGoal ();
          
          PNIFPGOOD ( goal );
          
        //printf ( " glide vel 01 = %f\n", mVel.get () );

          mVel *= ui::modOneDt ( mVelDamp, mGlideTracker.mDt );
          //mVel *= mVelDamp;
          
          PNIFPGOOD ( mVel );

        //printf ( " glide vel 02 = %f\n", mVel.get () );

          PNIFPGOOD ( mGlideTracker.mDt );

          mCurVal += mVel * mGlideTracker.mDt;

          PNIFPGOOD ( mCurVal );

          bool clamped = ! setCurVal ( mCurVal );
          
        //printf ( " curVal = = %f\n", mCurVal.get () );

          if ( pni::math::Trait::abs ( mVel ) <= mVelMin || clamped )
          {
            float dir = mVel > 0.0f ? 1.0f : -1.0f;
            
            PNIFPGOOD ( dir );
            PNIFPGOOD ( mVelMin );
            
            mVel = dir * mVelMin;
            
            PNIFPGOOD ( mVel );
            
            //if ( pni::math::Trait::abs ( goal ) > pni::math::Trait::abs ( mLastCurVal )
            //    && pni::math::Trait::abs ( goal ) < pni::math::Trait::abs ( mCurVal ) )

            PNIFPGOOD ( mItemSize );

            if ( mItemSize == 0.0f || clamped )
            {
              cancelGlide ();
            }
            else 
            {
              PNIFPGOOD ( mCurVal );
              PNIFPGOOD ( mLastCurVal );
              PNIFPGOOD ( goal );
            
              float dxCur = mCurVal - mLastCurVal;
              float dxGoal = goal - mLastCurVal;
              
              if ( pni::math::Trait::equal ( dxGoal, 0.0f ) )
              {
                mCurVal = goal;
                cancelGlide ();

                PNIFPGOOD ( mCurVal );
              }
              else
              {
                float dCurDgoal = dxCur / dxGoal;

                PNIFPGOOD ( dxGoal );
                PNIFPGOOD ( dxCur );
                PNIFPGOOD ( dCurDgoal );

                if ( dCurDgoal >= 1.0f )
                {
                  mCurVal = goal;
                  cancelGlide ();

                  PNIFPGOOD ( mCurVal );
                }
              }
            }
          }

          onWheelScroll ( this, event ( mGlideTracker.mCur ) );
        }
        
    void cancelGlide ()
        {
          if ( ! mGliding )
            return;
          mGliding = false;

          if ( ! mGoodDrag )
          {
            updateCurSelection ();
            
            onSelectionChanged ( this, event ( mGlideTracker.mCur ) );
          }
        }
        
    float calcGoal ()
        {
          PNIFPGOOD ( mItemSize );
        
          if ( mItemSize == 0.0f )
            return 0.0f;
        
          PNIFPGOOD ( mVel );
        
          float dir = mVel > 0.0f ? 1.0f : -1.0f;
          
          PNIFPGOOD ( dir );
          PNIFPGOOD ( mItemSize );
          PNIFPGOOD ( mCurVal );
          
          float under = mCurVal - pni::math::Trait::fmod ( mCurVal, mItemSize );
          
          PNIFPGOOD ( under );
          
          if ( mCurVal < 0.0f ) under = under - mItemSize;
          float over = under + mItemSize;
          
          PNIFPGOOD ( over );
          
          float goal = dir > 0.0f ? over : under;
          
          PNIFPGOOD ( goal );
          
//printf ( " curVal = %f, goal = %f\n", mCurVal.get (), goal );

          return goal;
        }

    float calcStaticVel ()
        {
          PNIFPGOOD ( mItemSize );
        
          if ( mItemSize == 0.0f )
            return 0.0f;

            // UNUSED, why? PRW
//          float dir = mVel > 0.0f ? 1.0f : -1.0f;
          
          PNIFPGOOD ( mCurVal );
          
          float under = mCurVal - pni::math::Trait::fmod ( mCurVal, mItemSize );
          if ( mCurVal < 0.0f ) under = under - mItemSize;
          float over = under + mItemSize;

          PNIFPGOOD ( under );
          PNIFPGOOD ( over  );
          
          if ( pni::math::Trait::abs ( mCurVal - under ) <
              pni::math::Trait::abs ( mCurVal - over ) )
            return -mVelMin;
          else
            return mVelMin;
        }

    ui::relativeTracker< float > mMouseTracker;
    ui::timeTracker mGlideTracker;

    pni::pstd::box< IndType > mCurSelection;
    pni::pstd::box< float > mCurVal;
    pni::pstd::box< float > mItemSize;
    pni::pstd::avg< float > mVelAvg;
    pni::pstd::box< float > mVel;
    pni::pstd::box< bool > mGoodDrag;
    pni::pstd::box< bool > mGliding;



};

/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuiwheelcore_h


