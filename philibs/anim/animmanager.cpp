/////////////////////////////////////////////////////////////////////
//
//    file: animmanager.cpp
//
/////////////////////////////////////////////////////////////////////

#include "animmanager.h"
#include "animevents.h"

/////////////////////////////////////////////////////////////////////

//#define DOANIMDBG
#ifdef DOANIMDBG

  #define DBGCALL(func,ptr) if ( mTrackBase==ptr ) func ( ptr );

  #include "pnilog.h"

#else

  #define DBGCALL(func,ptr)
  #define PNIPSTDLOGDISABLE
  #include "pnilog.h"

#endif

namespace anim {

/////////////////////////////////////////////////////////////////////

const TimeType manager::Now = 0.0f;
//const TimeType manager::After = -2.0f;
const TimeType manager::Forever = pni::math::Trait::maxVal;
  
/////////////////////////////////////////////////////////////////////

manager::manager() :
  mLastUpdate ( 0.0f )
{
  
}

manager::~manager()
{
  
}

// manager::manager(manager const& rhs)
// {
//   
// }
// 
// manager& manager::operator=(manager const& rhs)
// {
//   
//   return *this;
// }
// 
// bool manager::operator==(manager const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

bool manager::isStarting ( base* pBase, TimeType now )
{
  return ( pBase->getBounds ().mStart > mLastUpdate &&
      pBase->getBounds ().mStart <= now );
}

/////////////////////////////////////////////////////////////////////


void manager::update ( TimeType now )
{
  mFinalizeList.clear ();

  typedef Groups::iterator Iter;
  Iter end = mGroups.end ();
  for ( Iter cur = mGroups.begin (); cur != end;  )
  {
    group* pGroup = cur->second.get ();
    
      // Handle special cases like negative start times, etc.
    //if ( pGroup->getBounds ().mStart == After )
    //{
    //  // TODO
    //}
    //else 
    if ( pGroup->getBounds ().mStart <= 0.0f )
    {
      DBGCALL(onTrackBaseStartNegTime,pGroup)
      pGroup->getXform ().mStart = now - pGroup->getBounds ().mStart;
      pGroup->setXform ( pGroup->getXform () );
    }

// printf ( "pbase bounds = %f, %f\n", 
//     pGroup->getBounds ().mStart,
//     pGroup->getBounds ().mDuration );

      // See if now is within the range of the anim.
    if ( pGroup->contains ( now ) )
    {
      if ( isStarting ( pGroup, now ) )
      {
        DBGCALL(onTrackBaseStart,pGroup)
        // Emit onAnimStart event.
        animEvent event;
        event.setTimeStamp ( now );
        event.setBase ( pGroup );
        event.mManager = this;
        emitEvent ( pGroup, event, &animObserver::onAnimStart );
      }
    
      pGroup->update ( this, now );
      
      ++cur;
    }
      // Then see if the anim is done, requiring some cleanup work.
    else if ( pGroup->isDone ( now ) )
    {
      DBGCALL(onTrackBaseEnd,pGroup)
      TimeType val = pGroup->getBounds ().getEnd ();
      pGroup->update ( this, val );

      pni::pstd::autoRef< group > pSafety ( pGroup );

      if ( pGroup->getDoGc () )
      {
        DBGCALL(onTrackBaseRem,pGroup)
        Iter tmp = cur;
        ++cur;
        mGroups.erase ( tmp );
      }

      // Emit onAnimDone event.
      animEvent event;
      event.setTimeStamp ( now );
      event.setBase ( pSafety.get () );
      event.mManager = this;
      emitEvent ( pSafety.get (), event, &animObserver::onAnimDone );
    }
    else
      ++cur;
  }
  
  execFinalizeList ();
  
  mLastUpdate = now;
}

/////////////////////////////////////////////////////////////////////

void manager::execFinalizeList ()
{
  typedef Bases::iterator Iter;
  Iter end = mFinalizeList.end ();
  
  for ( Iter cur = mFinalizeList.begin (); cur != end; ++cur )
  {
    cur->second->finalize ();
  }
}

/////////////////////////////////////////////////////////////////////

void manager::addFinalize ( base* pBase )
{
  mFinalizeList[ pBase->getId () ] = pBase;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::addAnimObserver ( animObserver* pObs )
{
  mObservers.insert ( pObs );
}

void manager::remAnimObserver ( animObserver* pObs )
{
  mObservers.erase ( pObs );
}

/////////////////////////////////////////////////////////////////////

template< class Event, class Func >
void manager::emitEvent ( base* pBase, Event const& anEvent, Func func )
{
  if ( animObserver* pObs = pBase->getAnimObserver () )
    ( pObs->*func ) ( anEvent );

  Observers tmp ( mObservers );
  
  typedef Observers::iterator Iter;
  
  Iter end = tmp.end ();
  for ( Iter cur = tmp.begin (); cur != end; ++cur )
  {
    ( ( *cur )->*func ) ( anEvent );
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::addAnim ( group* pGroup )
{
    // Do we need to do anything special to any old anims we
    // are replacing, or is this enough?
    
    // Do we need to queue all adds so that any
    // anims added as a result of observer callbacks
    // are handled correctly and don't cause infinite
    // loops?
    
    // Is the criteria for uniqueness (getId call) really
    // good?  We could have five animations targeting the
    // same node concurrently, and they would all need 5 different ways
    // of indicating thier id... using the node ptr would
    // mean 4 of the 5 would be thrown away.  Maybe id
    // is a combo of an arbitrary id and the target ptr.
    // We should add set/get target to anim::base.
    // Yah, this is currently not that useful.
    
    // Also, this only looks for uniqueness of the group
    // passed in, not any of the base children.  Sigh.
    // Maybe we assume that the group ID is always symbolic,
    // e.g., "main screen anim" and never tied to an individual
    // node.  Note this also means that anim::base IDs are pretty
    // much useless.  Thusly, we needn't change anything.
    
    // Really, anim::manager should just be a specialization of
    // anim::group... but that would take a bit too much analysis
    // and refactoring for now.

//printf ( " new anim id = %d\n", pGroup->getId () );

  doRemEvent ( pGroup, false ); // removed implicitly by assignment below.

  DBGCALL(onTrackBaseAdd,pGroup)
  mGroups[ pGroup->getId () ] = pGroup;
}

void manager::remAnim ( group* pGroup )
{
  doRemEvent ( pGroup, true );
}

void manager::doRemEvent ( group* pGroup, bool andRemove )
{
  Groups::iterator found = mGroups.find ( pGroup->getId () );
  if ( found != mGroups.end () )
  {
    // Emit onAnimDone event.
    animEvent event;
    event.setTimeStamp ( mLastUpdate );
    event.setBase ( found->second.get () );
    event.mManager = this;
    emitEvent ( found->second.get (), event, &animObserver::onAnimDone );
    DBGCALL(onTrackBaseRem,found->second.get () )
    
    if ( andRemove )
      mGroups.erase ( found );
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class notifier :
  public anim::base
{
  public:
    virtual void internalUpdate ( manager* pManager, TimeType tval )
        {
          
        }
};

/////////////////////////////////////////////////////////////////////

void manager::addTimerNotification ( TimeType when, animObserver* pObs, idBase::Id id )
{
  notifier* pNotify = new notifier;
  pNotify->setXform ( xform ( 0.0f, pni::math::Trait::fuzzVal ) );
  
  group* pGrp = new group;
  pGrp->setId ( id );
  pGrp->addChild ( pNotify );
  pGrp->setAnimObserver ( pObs );
  pGrp->setXform ( xform ( when, 1.0f ) );
  
  addAnim ( pGrp );
}

/////////////////////////////////////////////////////////////////////

void manager::addSimpleAnim ( TimeType when, base* pAnim, animObserver* pObs, idBase::Id id )
{
  group* pGrp = new group;
  pGrp->setId ( id );
  pGrp->addChild ( pAnim );
  pGrp->setAnimObserver ( pObs );
  pGrp->setXform ( xform ( when, 1.0f ) );
  
  addAnim ( pGrp );
}

/////////////////////////////////////////////////////////////////////

void manager::cancelObserver ( animObserver* pObs )
{
    // First, walk all global observers and whack the ones that match.
  Observers::iterator end = mObservers.end ();
  for ( Observers::iterator cur = mObservers.begin ();
      cur != end;
       )
  {
    if ( *cur == pObs )
    {
      Observers::iterator tmp = cur;
      ++cur;
      mObservers.erase ( tmp );
    }
    else
    {
      ++cur;
    }
  }

    // Second, walk all groups and whack the ones that match.
    // TODO: This only walks groups right now, not individual
    // anim::bases.
  
  Groups::iterator gend = mGroups.end ();
  for ( Groups::iterator cur = mGroups.begin ();
      cur != gend;
      ++cur )
  {
    group* pGrp = cur->second.get ();
    if ( pGrp->getAnimObserver () == pObs )
      pGrp->setAnimObserver ( 0 );
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::gcHelpers ()
{
  typedef Helpers::iterator Iter;
  for ( Iter cur = mHelpers.begin ();  cur != mHelpers.end (); )
  {
    if ( cur->second->getNumRefs () == 1 )
    {
      Iter tmp = cur;
      ++cur;
      mHelpers.erase ( tmp );
    }
    else
      ++cur;
  }
}

/////////////////////////////////////////////////////////////////////

float manager::pow ( float val )
{
  const int Num = 256;
  static float vals[ Num ] = { 0.0 };
  static bool first = true;
  const float exp = 2.0f;   // Magic Number!!! 
                            // Ruh roh... this should be configurable.
  
    // TODO: Stop doing this cheesy static initialization
    // of a single power lookup table.  Move to something
    // more flexible so we can have an exponent other than
    // 2.0f.
  if ( first )
  {
    const float Div = 1.0f / ( Num - 1.0f );
    
    for ( int num = 0; num < Num; ++num )
    {
      float tmp = num;
      tmp *= Div;
      
      vals[ num ] = powf ( tmp, exp );
    }
    
    first = false;
  }
  
  int lb = val * ( Num - 1.0f );
  int ub = lb + 1;
  
  return vals[ lb ] * ( 1.0f - val ) + vals[ ub ] * val;
}

/////////////////////////////////////////////////////////////////////
// overrides from pni::pstd::refCount

void manager::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  pni::pstd::dbgRefCount::collectMapSecondRefs ( mGroups, refs );
  pni::pstd::dbgRefCount::collectMapSecondRefs ( mFinalizeList, refs );
  pni::pstd::dbgRefCount::collectMapSecondRefs ( mHelpers, refs );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::onTrackBaseAdd ( base* pBase )
{
PNIPSTDLOG
  onTrackBase ( pBase );
}

void manager::onTrackBaseRem ( base* pBase )
{
PNIPSTDLOG
  onTrackBase ( pBase );
}

void manager::onTrackBaseStart ( base* pBase )
{
PNIPSTDLOG
  onTrackBase ( pBase );
}

void manager::onTrackBaseStartNegTime ( base* pBase )
{
PNIPSTDLOG
  onTrackBase ( pBase );
}

void manager::onTrackBaseEnd ( base* pBase )
{
PNIPSTDLOG
  onTrackBase ( pBase );
}

void manager::onTrackBaseFinalize ( base* pBase )
{
PNIPSTDLOG
  onTrackBase ( pBase );
}

void manager::onTrackBaseReplace ( base* pBase )
{
PNIPSTDLOG
  onTrackBase ( pBase );
}

void manager::onTrackBase ( base* pBase )
{

}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace anim 


