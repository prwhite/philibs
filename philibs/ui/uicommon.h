/////////////////////////////////////////////////////////////////////
//
//  class: common
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuicommon_h
#define uiuicommon_h

/////////////////////////////////////////////////////////////////////

#include "pnivec3.h"
#include "pnibox3.h"

#include "uievents.h"

/////////////////////////////////////////////////////////////////////

namespace ui {
    
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

template< class Type >
class goalTracker
{
  public:
    
    goalTracker () :
      mRate ( 0.5f )
        {}
        
    goalTracker ( Type const& val ) :
      mCurVal ( val ),
      mRate ( 0.5f )
        {}

    void setCur ( Type const& val ) { mCurVal = val; }
    Type const& getCur () const { return mCurVal; }
    
    void setGoal ( Type const& val ) { mGoalVal = val; }
    Type const& getGoal () const { return mGoalVal; }
    
      // 0.0f == mCurVal, 1.0f == mGoalVal.
    void setRate ( float val ) { mRate = val; }
    float getRate () const { return mRate; }
    
    Type const& operator * () const { return mCurVal; }
    
    void calc ( ui::TimeType dt = 1.0f )
        {
            // TEMP:  Try to keep things under control, even when the
            // app is slowing down for whatever reason.
            // Magic Number!
          float const MaxDt = 1.0f / 30.0f;
          if ( dt > MaxDt )
            dt = MaxDt;
        
            // Optimize for things like vec3 that are better
            // with +=, *=, etc.
          //Type diff = mGoalVal - mCurVal;
            // Becomes...
          Type diff = mGoalVal;
          diff -= mCurVal;
          
            // TODO: Consider using threshold on absolute diff to
            // decide if we're close enough and stop doing lerp.
          
            // Then modulate in dt and rate which are commutative.
          diff *= dt * mRate;
          
            // Then add back to current value.
          mCurVal += diff;
        }
    
  private:
    Type mCurVal;
    Type mGoalVal;
    float mRate;
};

/////////////////////////////////////////////////////////////////////

struct relativeTrackerHelper
{
  static void clamp ( int& val, int minVal, int maxVal )
      { val = val < minVal ? minVal : val; val = val > maxVal ? maxVal : val; }
  static void clamp ( float& val, float minVal, float maxVal )
      { val = val < minVal ? minVal : val; val = val > maxVal ? maxVal : val; }
  static void clamp ( pni::math::vec2& val, pni::math::vec2 const& minVal, pni::math::vec2 const& maxVal )
      { clamp ( val[ 0 ], minVal[ 0 ], maxVal[ 0 ] );
        clamp ( val[ 1 ], minVal[ 1 ], maxVal[ 1 ] ); }
  static void clamp ( pni::math::vec3& val, pni::math::vec3 const& minVal, pni::math::vec3 const& maxVal )
      { clamp ( val[ 0 ], minVal[ 0 ], maxVal[ 0 ] );
        clamp ( val[ 1 ], minVal[ 1 ], maxVal[ 1 ] );
        clamp ( val[ 2 ], minVal[ 2 ], maxVal[ 2 ] ); }

  static void maxVal ( int& val ) { val = 0x7fffffff; }
  static void maxVal ( float& val ) { val = pni::math::Trait::maxVal; }
  static void maxVal ( pni::math::vec2& val )
      {  maxVal ( val[ 0 ] ); maxVal ( val[ 1 ] ); }
  static void maxVal ( pni::math::vec3& val )
      {  maxVal ( val[ 0 ] ); maxVal ( val[ 1 ] ); maxVal ( val[ 2 ] ); }

  static void zeroVal ( int& val ) { val = 0; }
  static void zeroVal ( float& val ) { val = 0.0f; }
  static void zeroVal ( pni::math::vec2& val )
      { zeroVal ( val[ 0 ] ); zeroVal ( val[ 1 ] ); }
  static void zeroVal ( pni::math::vec3& val )
      { zeroVal ( val[ 0 ] ); zeroVal ( val[ 1 ] ); zeroVal ( val[ 2 ] ); }

      
  static void oneVal ( int& val ) { val = 1; }
  static void oneVal ( float& val ) { val = 1.0f; }
  static void oneVal ( pni::math::vec2& val )
      { oneVal ( val[ 0 ] ); oneVal ( val[ 1 ] ); }
  static void oneVal ( pni::math::vec3& val )
      { oneVal ( val[ 0 ] ); oneVal ( val[ 1 ] ); oneVal ( val[ 2 ] ); }
      
  static void negate ( int& val ) { val = -val; }
  static void negate ( float& val ) { val = -val; }
  static void negate ( pni::math::vec2& val )
      { val[ 0 ] = -val[ 0 ]; val[ 1 ] = -val[ 1 ]; }
  static void negate ( pni::math::vec3& val )
      { val[ 0 ] = -val[ 0 ]; val[ 1 ] = -val[ 1 ]; val[ 2 ] = -val[ 2 ]; }
};

template< class Type >
class relativeTracker :
  protected relativeTrackerHelper
{
  public:
    relativeTracker () :
      mActive ( false )
        {
          oneVal ( mMult );
          clearMinMax ();

          Type tmp;
          zeroVal ( tmp );
          set ( tmp );
        }

    relativeTracker ( Type initVal ) :
      mActive ( false )
        {
          oneVal ( mMult );
          clearMinMax ();
          set ( initVal );
        }
    
    void onDown ( Type const& val )
        {
          mDown = val;
          onDrag ( val );
          mActive = true;
        }
        
    void onDrag ( Type const& val )
        {
          if ( ! mActive )
            return;
        
          mCur = val;

          mLastOut = mOut;
          mOut = mCur;
          mOut -= mDown;
          mOut += mLast;
          
          clamp ( mOut, mMin, mMax );
        }

    void onUp ( Type const& val )
        {
          if ( ! mActive )
            return;

          onDrag ( val );
          mActive = false;
          mLast = mOut;
        }

    void set ( Type const& val ) 
        { 
          Type tmp ( val );
          tmp *= mMult;
          
          mOut = mDown = mCur = mLast = mLastOut = tmp; 
          clamp ( mOut, mMin, mMax ); 
        }
    Type get () const { return mOut / mMult; }
    Type getDelta () { return ( mOut - mLastOut ) / mMult; }
    
    void setMinMax ( Type const& minVal, Type const& maxVal )
        {
          mMin = minVal * mMult; mMax = maxVal * mMult;
        }
        
    void setMult ( Type const& mult ) { mMult = mult; }
    Type const& getMult () const { return mMult; }
        
    void clearMinMax ()
        {
          maxVal ( mMin );
          negate ( mMin );
          maxVal ( mMax );
        }
        
    bool isActive () const { return mActive; }
    
  private:
    Type mDown;
    Type mCur;
    Type mLast;
    Type mLastOut;
    Type mOut;
    Type mMin;
    Type mMax;
    Type mMult;
    bool mActive;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class timeTracker
{
  public:
    typedef ui::TimeType TimeType;
    
    timeTracker ( TimeType duration ) : 
      mDuration ( duration ), mDurationInv ( 1.0f / duration ),
      mEnabled ( true ), mLastTvalStart ( -1.0f )
        { reset (); }
    
    void update ( TimeType timestamp )
        {
          if ( ! mEnabled )
            return;
        
          mLast = mCur;
          mLastTvalStart = mTvalStart;

          if ( mStart == 0.0f )
            mStart = timestamp;
          
          if ( mLast == 0.0f )
            mLast = timestamp;
          
          mCur = timestamp + mOffset;
          
          mDt = mCur - mLast;
          mDtStart = mCur - mStart;
          mTvalStart = mDtStart / mDuration;
        }
    
    void setDuration  ( TimeType val ) 
        { mDuration = val;  mDurationInv = 1.0f / val; }
    
    TimeType getDuration () const { return mDuration; }
    
    bool isActive () const 
        { return mEnabled && mTvalStart >= 0.0f && mTvalStart <= 1.0f; }
        
    bool justStarted () const 
        { return mEnabled && mTvalStart >= 0.0f && mLastTvalStart < 0.0f; }
    
    bool justTripped ( float val ) const
        { return mEnabled && mTvalStart >= val && mLastTvalStart < val; }
    
    void reset ( bool enabled = true )
        {
          mStart = 0.0f;
          mLast = 0.0f;
          mCur = 0.0f;
          mDt = 0.0f;
          mDtStart = 0.0f;
          mTvalStart = 0.0f;
          mLastTvalStart = -1.0f;
          mOffset = 0.0f;
          mEnabled = enabled;
        }
    
      // Public data!  
    TimeType mStart;
    TimeType mLast;
    TimeType mCur;
    TimeType mDt;
    TimeType mDtStart;
    TimeType mOffset;
    float mTvalStart;
    float mLastTvalStart;
    bool mEnabled;

  protected:
    TimeType mDuration;
    TimeType mDurationInv;
};


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

  // This helps a value converge to one when dt ~= 0.0
  // and to itself when dt ~= 1.0.  This can be used
  // to mult by a value that should move to one rather
  // than to zero.  E.g:
  //    scaleVal = scaleVal * modOnDt ( 0.9f, dt );
  // This is converging incoming scale value to 1.0 by
  // rate of 0.9 / second.

inline
float modOneDt ( float val, float dt )
{
  return 1.0f - ( 1.0f - val ) * dt;
}

/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuicommon_h


