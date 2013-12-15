/////////////////////////////////////////////////////////////////////
//
//  class: avg
//
/////////////////////////////////////////////////////////////////////

#ifndef pnipnipstdavg_h
#define pnipnipstdavg_h

/////////////////////////////////////////////////////////////////////

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace pni {
    namespace pstd {
        
/////////////////////////////////////////////////////////////////////

template< class Type >
class avg 
{
  public:
    avg ( size_t size = 32 ) :
      mSize ( size ),
      mSum ( ( Type ) 0 ),
      mCount ( 0 ),
      mCur ( 0 )
        {
          mVec.resize ( mSize, ( Type ) 0 );
        }
    
    void appendSample ( Type val )
        {
          ++mCount;
            // Increment to next sample... it's the oldest in the ring buffer.
          ++mCur;
          mCur %= mSize;
          
            // Getting rid of cur sample, so subtract that from sum.
          mSum -= mVec[ mCur ];
          
            // Now add in new sample value and stick it in the current spot.
          mSum += val;
          mVec[ mCur ] = val;
        }
        
    Type getCurAverage () const
        {
          size_t div = mCount < mSize ? mCount : mSize;
          
          if ( div != 0 )
            return mSum / div;
          else
            return Type ();
        }
        
    operator Type () const
        {
          return getCurAverage ();
        }
        
    void reset ()
        {
          mSum = 0;
          mCount = 0;
          mCur = 0;
          mVec.clear ();
          mVec.resize ( mSize );
        }
        
    bool isPrimed () const
        {
          return mCount >= mSize;
        }
    
  protected:

  private:
    typedef std::vector< Type > Vec;
    Vec mVec;
    Type mSum;
    size_t mSize;
    size_t mCount;
    size_t mCur;
};

/////////////////////////////////////////////////////////////////////

    } // end of namespace pstd 
} // end of namespace pni 

#endif // pnipnipstdavg_h


