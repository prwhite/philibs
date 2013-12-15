////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pniscenetypes_h
#define pniscenetypes_h

////////////////////////////////////////////////////////////////////

#include "pnimatrix4.h"
#include "pnibox3.h"

#include "sceneattr.h"

#include <vector>
#include "pnirefcount.h"
#include "pniautoref.h"

#ifdef WIN32
#pragma warning( disable : 4285 )
#endif

////////////////////////////////////////////////////////////////////

#ifndef PNIDBGDISABLE
  // Nada.
#else
#define QUIET
#endif

#ifdef QUIET
  #define PNIDBG
  #define PNIDBGSTR(str)
#else
  #ifndef _WIN32
    #define PNIDBG printf ( "%s:%d %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__ );
    #define PNIDBGSTR(str) printf ( "%s:%d %s: %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, str );
  #else
    #define PNIDBG printf ( "%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__ );
    #define PNIDBGSTR(str) printf ( "%s:%d %s: %s\n", __FILE__, __LINE__, __FUNCTION__, str );
  #endif
#endif

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

typedef float TimeType;

/////////////////////////////////////////////////////////////////////

enum Trav {
  Draw,
  Sound,
  Isect,
  UI,
  User00,
  User01,
  TravCount
};

class travDataContainer
{
  public:
    travDataContainer () { mTravData.resize ( TravCount ); }
  
    // TODO travDataContainer copy constructor!
  
    typedef pni::pstd::autoRef< attr > TravDataRef;
    typedef std::vector< TravDataRef > TravDatum;

    void setTravData ( Trav which, attr* pData ) { mTravData[ which ] = pData; }
    attr* getTravData ( Trav which ) const { return mTravData[ which ].get (); }
   
      // Exposed for refCount debugging.  Not for human consumption.
    TravDatum& getTravDatum () { return mTravData; }
   
  protected:
    TravDatum mTravData;
    
};

/////////////////////////////////////////////////////////////////////
// This is a matrix4 class that also keeps track of whether or not
// it is identity via an internal bool.  You can call through the
// class to other underlying methods via a smart pointer call:
//   matrix4 obj;
//   obj->preMult ( ... );

class matrix4 :
  public pni::math::matrix4
{
  public:
    matrix4 () :
      mIsIdent ( true )
        {
          
        }
    
    matrix4& operator = ( pni::math::matrix4 const& rhs )
        {
          pni::math::matrix4& self = *this;
          self = rhs;
          return *this;
        }
    
    // TODO need more constructors from pni::scene::matrix4
    
    void setIsIdent ( bool val ) { mIsIdent = val; }
    bool getIsIdent () const { return mIsIdent; }
    
    matrix4& operator -> ()
        {
          setIsIdent ( false );
          return *this;
        }
    
    // Hides and dispatches to definition from pni::math::matrix4.
    void setIdentity ()
        {
          setIsIdent ( true );
          pni::math::matrix4::setIdentity ();
        }
    
    // Um... this might be too cute.
    operator bool () const { return mIsIdent; }
    
    bool mIsIdent;    
};

////////////////////////////////////////////////////////////////////

class box3 :
  public pni::math::box3
{
  public:
    box3 () :
      mIsDirty ( true )
        {
          
        }
        
    // TODO need more constructors from pni::scene::matrix4
    
    void setIsDirty ( bool val ) { mIsDirty = val; }
    bool getIsDirty () const { return mIsDirty; }
    
    operator bool () const { return mIsDirty; }
    
    bool mIsDirty;
};

/////////////////////////////////////////////////////////////////////

template< class Type >
pni::math::Trait::ValueType fastLength ( Type const& vec, int iter = 2 )
{
  return 1.0f / pni::math::Trait::fastInvSqrt ( vec.dot ( vec ), iter );
}

template< class Type >
void fastNormalize ( Type& vec, int iter = 2 )
{
  vec *= pni::math::Trait::fastInvSqrt ( vec.dot ( vec ), iter );
}

/////////////////////////////////////////////////////////////////////
  // Composes a matrix with a specific ordered semantic.
  
//class matrix4trs :
//  public pni::math::matrix4
//{
//  public:
//    matrix4trs ( pni::math::matrix4 const& rhs ) :
//      pni::math::matrix4 ( rhs )
//          {}
//          
//    void setTrans ( pni::math::vec3 const& trans ) { mTrans = trans; }
//        
//      // This rotation vector is similar to the euler
//      // representation, but it's composed in a different
//      // order.
//    void setRot ( pni::math::vec3 const& rot ) { mRot = rot; }
//
//    void setScale ( pni::math::vec3 const& scale ) { mScale = scale; }
//
//    void update ()
//        {
//          // TODO
//        }
//};

/////////////////////////////////////////////////////////////////////

} // end namespace scene

#endif // pniscenetypes_h
