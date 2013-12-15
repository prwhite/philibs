/////////////////////////////////////////////////////////////////////
//
//  
//
/////////////////////////////////////////////////////////////////////

#ifndef sceneparticleactions_h
#define sceneparticleactions_h

/////////////////////////////////////////////////////////////////////

#include "sceneparticle.h"

#include "pnirand.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

class velApplyAction :
  public scene::particles::action
{
  public:
    velApplyAction ( float damp = 1.0f ) :
      mDamp ( damp ) {}
  
    virtual void update ( 
        scene::graphDd::fxUpdate const& update, scene::particles* pParts )
        {
          scene::spriteData::iterator iter ( &pParts->getSpriteData () );
          
          while ( iter )
          {
            scene::particles::particle* pPart = 
                iter.get< scene::particles::particle > ();
            
            pPart->mVel *= 1.0f - ( 1.0f - mDamp ) * update.mDt;
            pPart->mPos += pPart->mVel * update.mDt;
          
            ++iter;
          }
        }
        
      // Public data.
    float mDamp;
};

/////////////////////////////////////////////////////////////////////

class velTrailAction :
  public scene::particles::action
{
  public:
    struct args
        {
          args () :
            mVelDamp ( 1.0f ), mSpawnTime ( 1.0f / 30.0f ), mAgeMod ( 1.0f ),
            mSizeMod ( 1.0f ), mIterations ( 7 ), mSkipFirst ( false ) {}
          float mVelDamp;     // [0,1]
          float mSpawnTime;   // seconds... ex: 1/30
          float mAgeMod;      // [0,1-ish]
          float mSizeMod;     // [0,1-ish]
          float mMinVar;      // [0,1-ish]
          float mMaxVar;      // [0,1-ish]
          size_t mIterations; // [1,oo]
          bool mSkipFirst;    // false
        };
  
    velTrailAction ( args const& argsIn ) :
      mArgs ( argsIn ), mCurIteration ( 0 ), mLastSpawnTime ( 0.0f ),
      mStart ( 0 ), mCount ( 0 ) 
        {
          if ( mArgs.mSkipFirst )
            mStart = 1;
        }

    virtual void update ( graphDd::fxUpdate const& update, particles* pParts )
        {
          if ( ! ( mCurIteration < mArgs.mIterations ) )
            return;
            
          if ( update.mTimeStamp - mLastSpawnTime < mArgs.mSpawnTime )
            return;
                
          scene::spriteData* pData = &pParts->getSpriteData ();
          size_t const elemCount = pData->getElemCount ();
          
            // Keep track of original particle count... we will add this
            // many particles each iteration.
          if ( mCount == 0 )
          {
            mCount = elemCount;
            if ( mStart == 1 )
              --mCount;
            mLastSpawnTime = update.mTimeStamp;
          }

            // Protect against underrun on the array as it gets empty.
          if ( elemCount < mCount )
          {
            mCurIteration = mArgs.mIterations;
            return;
          }

          pData->resize ( elemCount + mCount );

            // We have to set this every time because a reap action
            // could have caused the original particle array to 
            // shift down, making the last setting of mStart silly.
            // It kinda shouldn't be a member variable.
          mStart = elemCount - mCount;

          Iter src ( pData );
          src += mStart;
          Iter dst ( pData );
          dst += mStart += mCount;
          
          scene::TimeType dt = update.mTimeStamp - mLastSpawnTime;
          
          while ( dst )
          {
            scene::particles::particle* pSrc = 
                src.get< scene::particles::particle > ();
            scene::particles::particle* pDst = 
                dst.get< scene::particles::particle > ();
            
            float var = pni::math::getTheRand () ( mArgs.mMinVar, mArgs.mMaxVar );
            
            *pDst = *pSrc;
            pDst->mVel *= modOneDt ( mArgs.mVelDamp, dt );
            pDst->mPos += pDst->mVel * dt;
            pDst->mAge *= modOneDt ( mArgs.mAgeMod, dt ) * var;
            pDst->mSize *= modOneDt ( mArgs.mSizeMod, dt ) * var;
            //pDst->mFrame = 0.0f;
            
            ++src;
            ++dst;
          }
          
          mStart += mCount;
          ++mCurIteration;
          mLastSpawnTime = update.mTimeStamp;
        }

    float modOneDt ( float val, float dt )
        {
          return 1.0f - ( 1.0f - val ) * dt;
        }

    args mArgs;
    size_t mCurIteration;
    size_t mStart;
    size_t mCount;
    scene::TimeType mLastSpawnTime;
};

/////////////////////////////////////////////////////////////////////

class frameAnimAction :
  public scene::particles::action
{
  public:
    frameAnimAction ( float duration ) :
      mDuration ( duration ) {}
  
    virtual void update ( 
        scene::graphDd::fxUpdate const& update, scene::particles* pParts )
        {
          scene::spriteData::iterator iter ( &pParts->getSpriteData () );
          
          size_t numFrames = pParts->getNumFrames ();
            
          while ( iter )
          {
            scene::particles::particle* pPart = 
                iter.get< scene::particles::particle > ();
            
            size_t frame = ( size_t ) ( numFrames * pPart->mAge / mDuration );
            
            if ( frame >= numFrames )
              frame = numFrames - 1;
            
            pPart->mFrame = ( float ) frame;
          
            ++iter;
          }
        }
        
      // Public data.
    float mDuration;
};

/////////////////////////////////////////////////////////////////////

class ageAction :
  public scene::particles::action
{
  public:
    ageAction ( float minVariance = 1.0f, float maxVariance = 1.0f ) :
      mMinVariance ( minVariance ), mMaxVariance ( maxVariance ) {}
  
    virtual void update ( 
        scene::graphDd::fxUpdate const& update, scene::particles* pParts )
        {
          scene::spriteData::iterator iter ( &pParts->getSpriteData () );
          
          while ( iter )
          {
            scene::particles::particle* pPart = 
                iter.get< scene::particles::particle > ();
                
            pPart->mAge += update.mDt * 
                pni::math::getTheRand () ( mMinVariance, mMaxVariance );
          
            ++iter;
          }
        }
        
      // Public data.
    float mMinVariance;
    float mMaxVariance;
};


/////////////////////////////////////////////////////////////////////

class reapAction :
  public scene::particles::action
{
  public:
    reapAction ( float duration ) :
      mDuration ( duration ) {}
  
    virtual void update ( 
        scene::graphDd::fxUpdate const& update, scene::particles* pParts )
        {
          scene::spriteData* pData = &pParts->getSpriteData ();
          scene::spriteData::iterator iter ( pData );
          
          size_t num = 0;
          while ( iter )
          {
            scene::particles::particle* pPart = 
                iter.get< scene::particles::particle > ();
                
            if ( pPart->mAge > mDuration )
              mReapVec.push_back ( num );
          
            ++iter;
            ++num;
          }
          
            // Iterate back to front to make erase a bit more efficient
            // and for the reap indices to not be invalidated.  
          ReapVec::reverse_iterator end = mReapVec.rend ();
          for ( ReapVec::reverse_iterator cur = mReapVec.rbegin ();
              cur != end;
              ++cur )
            pData->remElemAt ( *cur );
          
          mReapVec.clear ();
        }
    
    typedef std::vector< size_t > ReapVec;
    
      // Public data.
    ReapVec mReapVec;
    float mDuration;
};




/////////////////////////////////////////////////////////////////////

class gravAction :
  public scene::particles::action
{
  public:
    gravAction ( float grav = -9.8 ) :
      mGrav ( grav ) {}
  
    virtual void update ( 
        scene::graphDd::fxUpdate const& update, scene::particles* pParts )
        {
          scene::spriteData::iterator iter ( &pParts->getSpriteData () );
          
          while ( iter )
          {
            scene::particles::particle* pPart = 
                iter.get< scene::particles::particle > ();
                
            pPart->mVel[ 2 ] += mGrav * update.mDt;
          
            ++iter;
          }
        }
        
      // Public data.
    float mGrav;
};


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class convertAction :
  public scene::particles::action
{
  public:
    convertAction () {}
  
    virtual void update ( 
        scene::graphDd::fxUpdate const& update, scene::particles* pParts )
        {
          scene::spriteData::iterator iter ( &pParts->getSpriteData () );
          
          while ( iter )
          {
            scene::particles::particle* pPart = 
                iter.get< scene::particles::particle > ();
                
            //pPart->mVel[ 2 ] += mGrav * update.mDt;
            
            pPart->mPos.xformPt4 ( pPart->mPos, mMatrix );
          
            ++iter;
          }
        }
    
      // Public data.
    pni::math::matrix4 mMatrix;
};


/////////////////////////////////////////////////////////////////////

} // end namespace scene

/////////////////////////////////////////////////////////////////////

#endif // sceneparticleactions_h

