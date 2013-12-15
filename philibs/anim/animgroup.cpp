/////////////////////////////////////////////////////////////////////
//
//    file: animgroup.cpp
//
/////////////////////////////////////////////////////////////////////

#include "animgroup.h"
#include "animmanager.h"

#include "pnimath.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
  
/////////////////////////////////////////////////////////////////////

group::group() :
  mDirty ( true ),
  mLastTval ( 0.0f )
{
  setId ( this ); // Kinda horrible default considring how many things
                  // can unexpectedly happen if this is never changed.
}

// group::~group()
// {
//   
// }
// 
// group::group(group const& rhs)
// {
//   
// }
// 
// group& group::operator=(group const& rhs)
// {
//   
//   return *this;
// }
// 
// bool group::operator==(group const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////

void group::internalUpdate ( manager* pManager, TimeType tval )
{
  typedef std::vector< Bases::iterator > Done;
  Done done;
  
  typedef Bases::iterator Iter;
  Iter end = mBases.end ();
  for ( Iter cur = mBases.begin (); cur != end; ++cur )
  { 
    base* pBase = ( *cur ).get ();
    
      // TODO: This should have most of the same logic as
      // anim::manager::update, including:
      // Detecting animation starts, emitting an event.
      // Updating animations that are current.
      // Detecting animation ends, emitting an event, 
      // tossing the animation.
    
    if ( pBase->getBounds ().contains( tval ) )
    {
      pBase->update ( pManager, tval );
      
      if ( pBase->needsFinalize () )
        pManager->addFinalize ( pBase );
    }
    else if ( pBase->isDone ( tval ) )
    {
      pBase->update ( pManager, pBase->getBounds ().getEnd () );

      if ( pBase->needsFinalize () )
        pManager->addFinalize ( pBase );
      
      if ( pBase->getDoGc () )
        done.push_back ( cur );
    }
  }
  
  typedef Done::reverse_iterator Riter;
  Riter rend = done.rend ();
  for ( Riter rcur = done.rbegin (); rcur != rend; ++rcur )
    mBases.erase ( *rcur );
}

/////////////////////////////////////////////////////////////////////

void group::updateBounds () const
{
  if ( mDirty )
  {
    mBounds.invalidate ();

    typedef Bases::const_iterator Iter;
    Iter end = mBases.end ();
    for ( Iter cur = mBases.begin (); cur != end; ++cur )
    {
      base const* pBase = ( *cur ).get ();
      
      bounds tbounds ( pBase->getBounds () );
    
      mBounds.extendBy ( tbounds );
    }
    
    xform txform ( mXform );
    mBounds.transform ( mXform );
  }

  setDirty ( false );
}

/////////////////////////////////////////////////////////////////////

void group::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  base::collectRefs ( refs );
  pni::pstd::dbgRefCount::collectVecRefs ( mBases, refs );
}

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 


