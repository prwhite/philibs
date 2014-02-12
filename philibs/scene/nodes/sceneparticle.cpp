/////////////////////////////////////////////////////////////////////
//
//    file: sceneparticle.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneparticle.h"
#include "pnirefcountdbg.h"
#include <cassert>

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

particles::particles () :
  mLastUpdate ( 0.0f )
{
  mData.addBinding ( Vel, 3 );
  mData.addBinding ( Age, 1 );
  mData.addBinding ( Bits, sizeof ( BitsType ) / 4 );
  
  assert ( mData.getStride () * 4 == sizeof ( particle ) );
  
  //setBoundsMode ( ForceEmpty );
}

//particles::~particles ()
//{
//  // TODO
//}
//
//particles::particles ( particles const& rhs )
//{
//  // TODO
//}
//
//particles& particles::operator= ( particles const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool particles::operator== ( particles const& rhs ) const
//{
//  // TODO
//  return false;
//}

/////////////////////////////////////////////////////////////////////

void particles::updateParticles ( graphDd::fxUpdate const& update )
{
  typedef Actions::iterator Iter;
  Iter end = mActions.end ();
  for ( Iter cur = mActions.begin (); cur != end; ++cur )
    ( *cur )->update ( update, this );
}

/////////////////////////////////////////////////////////////////////
//  overrides from sprite

void particles::update ( graphDd::fxUpdate const& update )
{
    // Because these nodes can be instanced, we want to make
    // sure we don't evaluate the bahavior more than once per frame.
  if ( mLastUpdate != update.mTimeStamp )
  {
    updateParticles ( update );
    mLastUpdate = update.mTimeStamp;
  }
    
  sprites::update ( update );

  // TODO
}

/////////////////////////////////////////////////////////////////////

void particles::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  sprites::collectRefs ( refs );
  
  pni::pstd::dbgRefCount::collectVecRefs ( mActions, refs );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


