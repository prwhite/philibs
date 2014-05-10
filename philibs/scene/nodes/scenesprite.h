/////////////////////////////////////////////////////////////////////
//
//  class: sprites
//
//  This is super-simplified compared to the pni equivalent class.
//  The up vector is always in local space.
//  The 'mode' is always 'point', not one of the cardinal axes.
//  The rotation is always to a global viewer (direction).
//  There are no per-sprite rotations.
//  There is no per-sprite color. [Currently]
//
// TODO: Consider dirty flag... don't need to update sprites
// if no items have changed and the camera hasn't changed.
// as optimizations go, I don't think it will be much of a win.

/////////////////////////////////////////////////////////////////////

#ifndef scenescenesprites_h
#define scenescenesprites_h

/////////////////////////////////////////////////////////////////////

#include "scenegeomfx.h"

#include "pnivec3.h"
#include "pnivec2.h"
#include "pnimatrix4.h"

#include <vector>
#include <algorithm>

/////////////////////////////////////////////////////////////////////

namespace scene {

  class texture;

/////////////////////////////////////////////////////////////////////

class spriteData 
//:
  //public pni::pstd::refCount
{
  public:
    
    enum BindingVals
    {
      BindingError = ~0x00
    };
  
    spriteData () :
      mStride ( 0 )
        {}
  
    typedef unsigned int BindingType;

      // Currently setting the bindings is not random access... only
      // in-order via add (push_back).
    size_t addBinding ( BindingType binding, size_t size )
        {
          size_t which = mBindings.size ();
          mBindings.resize ( which + 1 );
          mBindings[ which ].mBinding = binding;
          mBindings[ which ].mSize = size;
          updateOffsets ();
          return which;
        }
        
    void getBinding ( size_t which, BindingType& binding, size_t& size )
        {
          binding = mBindings[ which ].mBinding;
          size = mBindings[ which ].mSize;
        }
        
    size_t getStride () const { return mStride; }
    
    void resize ( size_t numElems ) { mData.resize ( mStride * numElems ); }
    void clear () { mData.clear (); }
    size_t getElemCount () const { return mData.size () / mStride; }
    
    void remElemAt ( size_t elem )
        {
          DataVec::iterator beg = mData.begin ();
          beg += mStride * elem;
          
          DataVec::iterator end = beg;
          end += mStride;
          
          mData.erase ( beg, end );
        }
    
    size_t getOffset ( size_t which ) const
        {
          return mBindings[ which ].mOffset;
        }

    size_t getSemanticOffset ( BindingType which ) const
        {
            // Find offset of particular attribute.
          size_t end = mBindings.size ();
          for ( size_t num = 0; num < end; ++num )
            if ( mBindings[ num ].mBinding == which )
              return mBindings[ num ].mOffset;
            
          return BindingError;
        }
        
      // Get ptr for attribute... uses slot, not semantic identifier.
    float* getBindingPtr ( size_t which, size_t count = 0 )
        {
          if ( ! mData.empty () )
          {
            float* start = &mData[ getOffset ( which ) ];
            start += count * mStride;
            return start;
          }
          else
            return 0;
        }
        
    float const* getBindingPtr ( size_t which, size_t count = 0 ) const
        {
          if ( ! mData.empty () )
          {
            float const* start = &mData[ getOffset ( which ) ];
            start += count * mStride;
            return start;
          }
          else
            return 0;
        }

      // Get ptr for attribute... uses semantic id.
    float* getSemanticBindingPtr ( BindingType which, size_t count = 0 )
        {
          float* start = &mData[ getSemanticOffset ( which ) ];
          start += count * mStride;
          return start;
        }
        
    float const* getSemanticBindingPtr ( BindingType which, size_t count = 0 ) const
        {
          float const* start = &mData[ getSemanticOffset ( which ) ];
          start += count * mStride;
          return start;
        }
        
      // Iterator for user-defined types to overlay the
      // vector of floats.  Currently only intended for forward
      // increment, not random access.  Don't change the size of the
      // vector during iteration as the termination condition is
      // based on a cached version of the end.
    class iterator
        {
          public:
            iterator ( spriteData* pData ) :
              mData ( pData ),
              mPtr ( pData->getBindingPtr ( 0 ) ),
              mEnd ( pData->getBindingPtr ( 0, pData->getElemCount () ) )
                {}
            iterator ( iterator const& rhs ) :
              mData ( rhs.mData ),
              mPtr ( rhs.mPtr ),
              mEnd ( rhs.mEnd )
                {}
                
            iterator& operator ++ ()
                {
                  mPtr += mData->getStride ();
                  return *this;
                }
                
            iterator operator += ( size_t count )
                {
                  mPtr += mData->getStride () * count;
                  return *this;
                }
                
            operator bool () const 
                {
                  //return mPtr != mEnd;
                  return mPtr < mEnd;
                }
                
              // This is the goods right here.  Use explicit template
              // parameter in function invocation to get the cur address
              // ass the type 'Type'.
            template< class Type >
            Type* get ()
                {
                  return reinterpret_cast< Type* > ( mPtr );
                }
                            
          private:
            spriteData* mData;
            float* mPtr;
            float* mEnd;
        };
  
  private:
  
    struct binding
        {
          binding () :
            mBinding ( 0 ),
            mSize ( 0 ),
            mOffset ( 0 )
                {}
                
          BindingType mBinding;
          size_t mSize;         // Not in bytes, but in sizeof ( float ).
          size_t mOffset;       // Not in bytes, but in sizeof ( float ).
        };
    
    void updateOffsets ()
        {
          size_t curSize = 0;
          size_t end = mBindings.size ();
          for ( size_t num = 0; num < end; ++num )
          {
            mBindings[ num ].mOffset = curSize;
            curSize += mBindings[ num ].mSize;
              
          }

          mStride = curSize;
        }
    
    typedef std::vector< float > DataVec;
    typedef std::vector< binding > BindingVec;
    
    DataVec mData;
    BindingVec mBindings;
    size_t mStride;       // Not in bytes, but in sizeof ( float ).
};
    
/////////////////////////////////////////////////////////////////////

class sprites :
  public geomFx
{
  public:
    sprites ();
    //virtual ~sprites ();
    //sprites ( sprites const& rhs );
    //sprites& operator= ( sprites const& rhs );
    //bool operator== ( sprites const& rhs ) const;

    enum SpriteBindings
    {
      Position = 0,     // x, y, z
      Size = 1,         // x, z
      Flip = 2,         // x, z (-1|1)
      Frame = 3,        // float, but quantized to integer value
      NextSpriteBinding = 4
    };

      // Use this class via a pointer as an overlay for the spriteData buffer.    
    struct sprite
    {
      protected:
        sprite () {}  // Can't instantiate this.
        
      public:
        pni::math::vec3 mPos;
        pni::math::vec2 mSize;
        pni::math::vec2 mFlip;  // Should be {-1,1} to flip UVs.
        float mFrame;  // Which frame of multi-frame sprite texture.
    };

    
    spriteData& getSpriteData () { return mData; }
    spriteData const& getSpriteData () const { return mData; }
    
    void setNumFrames ( size_t num ) { mNumFrames = num; mFrameDiv = 1.0f / num; }
    size_t getNumFrames () const { return mNumFrames; }
    
    void setDoDepthSort ( bool val ) { mDoDepthSort = val; }
    bool getDoDepthSort () const { return mDoDepthSort; }
    
    void setFrozen ( bool val ) { mFrozen = val ? 1 : 0; }
    bool getFrozen () const { return mFrozen ? true : false; }
    
    void makeSimpleSprite ( pni::math::vec2 const& size, texture* pTex );
    spriteData::iterator getSimpleIterator () 
        { return spriteData::iterator ( &mData ); }
    
  protected:

    struct sorter
        {
          sorter ( geomData::IndexType index, float distSqr ) :
            mIndex ( index ), mDistSqr ( distSqr )
              {}
          
          bool operator < ( sorter const& rhs ) const
              {
                // TRICKY: We are reversing the < to > so we get
                // a back-to-front sort which is key for blending.
                return mDistSqr > rhs.mDistSqr;
              }
              
          geomData::IndexType mIndex;
          float mDistSqr;
        };

    typedef std::vector< sorter > Sorters;
  
    void calcRotMat ( graphDd::fxUpdate const& update,
        pni::math::matrix4& mat );
    void calcXYvecs ( pni::math::matrix4 const& mat );
    void doDepthSort ( graphDd::fxUpdate const& update,
        Sorters& sorters );
    void addSpriteToGeom ( geomData::IndexType dst, geomData::IndexType src );
  
    pni::math::vec3 mXvec;  // Used internally during update calculations
    pni::math::vec3 mYvec;  // Used internally during update calculations
    
    //Sprites mItems;
    spriteData mData;
    size_t mNumFrames;    // Default to 1.
    float mFrameDiv;      // 1 / mNumFrames.
    size_t mFrozen;       // Default to 0;
    bool mDoDepthSort;    // Defaults to true.

  private:


    // interface from geomFx
  public:
    virtual void update ( graphDd::fxUpdate const& update );
    
    virtual void generateGeomBounds () const;
    
  protected:
		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenesprites_h


