/////////////////////////////////////////////////////////////////////
//
//    file: animscenematerial.cpp
//
/////////////////////////////////////////////////////////////////////

#include "animscenematerial.h"

#include <algorithm>

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

sceneMaterial::sceneMaterial () :
  mProp ( Diffuse )
{
  // TODO
}

//sceneMaterial::~sceneMaterial ()
//{
//  // TODO
//}
//
//sceneMaterial::sceneMaterial ( sceneMaterial const& rhs )
//{
//  // TODO
//}
//
//sceneMaterial& sceneMaterial::operator= ( sceneMaterial const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool sceneMaterial::operator== ( sceneMaterial const& rhs ) const
//{
//  // TODO
//  return false;
//}

void sceneMaterial::initStartFromCur ()
{
  key tmp;
  tmp.mTimeStamp = 0.0f;
  
  switch ( mProp )
  {
    default:
    case Diffuse:
      tmp = mMat->getDiffuse ();
      break;
    case Ambient:
      tmp = mMat->getAmbient ();
      break;
    case Specular:
      tmp = mMat->getSpecular ();
      break;
    case Emissive:
      tmp = mMat->getEmissive ();
      break;
    case Shininess:
      tmp[ 0 ] = mMat->getShininess ();
      break;
  }
  
  mKeys.insert ( mKeys.begin (), tmp );
}

/////////////////////////////////////////////////////////////////////
//  overrides from anim::base

void sceneMaterial::updateBounds () const
{
  sceneMaterial* pSelf = const_cast< sceneMaterial* > ( this );
  pSelf->reSortKeys ();
  
  if ( ! mKeys.empty () )
    mBounds.set ( xform ( mKeys.front ().mTimeStamp, 
        mKeys.back ().mTimeStamp - mKeys.front ().mTimeStamp ) );
  
    // Hmmm... maybe the comment in base::updateBounds says we
    // need things in parent frame, we should mult this out by
    // the xform duration.  
  mBounds.transform ( mXform );
  
  setDirty ( false );
}

/////////////////////////////////////////////////////////////////////

void sceneMaterial::reSortKeys ()
{
  std::sort ( mKeys.begin (), mKeys.end () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void sceneMaterial::internalUpdate ( manager* pManager, TimeType tval )
{
  if ( mKeys.size () < 2 )
    return;

  //if ( getExponent () > 1.0f )
    //tval = useExponent ( pManager, tval );

    // Handle interp between appropriate keys.
  key out;
  out.mTimeStamp = tval;

//for ( size_t num = 0; num < mKeys.size (); ++num )
//  printf ( "key %d = %f\n", num, mKeys[ num ].mTimeStamp );

  //Range range = equal_range ( mKeys.begin (), mKeys.end (), out );
  Keys::iterator next = lower_bound ( 
      mKeys.begin (), mKeys.end (), out );
  Keys::iterator prev = next;
  if ( prev != mKeys.begin () )
    --prev;
  else
    return;

  float mdiff = next->mTimeStamp - prev->mTimeStamp;
  float mtval = tval / mdiff;
  
  //pni::math::vec4 out ( pni::math::vec4::NoInit );
  out.combine ( 1.0f - mtval, *prev, mtval, *next );

//printf ( "%x tval = %f, mtval = %f, mdiff = %f\n", this, tval, mtval, mdiff );
  
  applyValue ( out );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

void sceneMaterial::applyValue ( pni::math::vec4 const& val )
{
  switch ( mProp )
  {
    default:
    case Diffuse:
      mMat->setDiffuse ( val );
      break;
    case Ambient:
      mMat->setAmbient ( val );
      break;
    case Specular:
      mMat->setSpecular ( val );
      break;
    case Emissive:
      mMat->setEmissive ( val );
      break;
    case Shininess:
      mMat->setShininess ( val[ 0 ] );
      break;
  }
}

/////////////////////////////////////////////////////////////////////

void sceneMaterial::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  base::collectRefs ( refs );
  refs.push_back ( mMat.get () );
}

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 


