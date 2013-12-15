/////////////////////////////////////////////////////////////////////
//
//    file: uimanager.cpp
//
/////////////////////////////////////////////////////////////////////

#include "uimanager.h"
#include "uimanagerevents.h"

#include "sceneisectdd.h"
#include "sceneconverter.h"

#include "pnirefcountdbg.h"

//#undef PNIDBG
//#define PNIDBG

/////////////////////////////////////////////////////////////////////

namespace ui {

/////////////////////////////////////////////////////////////////////

void layer::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  refs.push_back ( mRoot.get () );
  
  mCamPath.collectRefs ( refs );
}

/////////////////////////////////////////////////////////////////////

  // Just to make sure this is instantiated somewhere.
eventObserver::~eventObserver()
{
  
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

layer::~layer ()
{
  if ( mUiMgr )
    mUiMgr->unref ();
    
  // We don't own the mDd or mSndDd... just alias them.
  // They are deleted in app::~app.
}

void layer::setUiManager ( manager* pUiMgr )
{
  manager*  pOldUiMgr = mUiMgr;

  mUiMgr = pUiMgr;
  if ( mUiMgr )
    mUiMgr->ref ();
  
  if ( pOldUiMgr )
    pOldUiMgr->unref ();
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

manager::manager() :
  mCurTime ( 0.0f ),
  mThisFrameStartTime ( 0.0f ),
  mLastFrameFinishTime ( 0.0f )
{
  mFlickObserver = new flickObserver ( this );
  addEventObserver ( mFlickObserver );
  
  mClickObserver = new clickObserver ( this );
  addEventObserver ( mClickObserver );
  
  mTimer.setOffset ( -mTimer.get () );
}

manager::~manager()
{
  remEventObserver ( mFlickObserver );
  delete mFlickObserver;
  mFlickObserver = 0;
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

//#define DBGOBSERVERS

void manager::addEventObserver ( eventObserver* pObserver )
{
  mObservers.insert ( pObserver );

#ifdef DBGOBSERVERS
  printf ( "  ### ui::manager new event observer registered:\n" );
  printf ( "    %x is a %s\n", pObserver, typeid ( *pObserver ).name () );
#endif  // DBGOBSERVERS
}

void manager::remEventObserver ( eventObserver* pObserver )
{
  mObservers.erase ( pObserver );
  
#ifdef DBGOBSERVERS
  printf ( "  *** removing event observer %x of type %s\n", pObserver,
      typeid ( *pObserver ).name () );
  printf ( "    ui::manager event observers still registered:\n" );
  
  Observers::iterator end = mObservers.end ();
  for ( Observers::iterator cur = mObservers.begin ();
      cur != end;
      ++cur )
  {
    eventObserver* pObs = *cur;
    printf ( "  %x is a %s\n", pObs, typeid ( *pObs ).name () );
  }
#endif // DBGOBSERVERS
}

template< class Event, class Func >
void manager::emitEvent ( Event const& anEvent, Func func )
{
  typedef Observers::const_iterator Iter;
  Observers tmp ( mObservers );
  
  Iter end = tmp.end ();
  for ( Iter cur = tmp.begin (); cur != end; ++cur )
  {
    // This is _way_ inefficient... but we have to protect against
    // invoking observers that have been removed during the callbacks
    // from this method.
    // TODO: Do something more sensible.
    if ( mObservers.find ( *cur ) != mObservers.end () )
      ( ( *cur )->*func ) ( anEvent );
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::onMouseDown ( mouseEvent const& event )
{
  mouseEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );
  if ( doIsect ( tmpEvent ) )
  {
    mCurBase = tmpEvent.getBase ();
    mTextFocusBase = tmpEvent.getBase ();  
    
    if ( mCurBase )
      mCurBase->onMouseDown ( tmpEvent );
  }
  
  emitEvent ( tmpEvent, &ui::eventObserver::onMouseDown );
}

/////////////////////////////////////////////////////////////////////

void manager::onMouseDrag ( mouseEvent const& event )
{
  mouseEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );
  
  doIsect ( tmpEvent );
  //if ( doIsect ( tmpEvent ) )
  //{
    // Only mouseDown can change the focused widget.
//     mCurBase = tmpEvent.getBase ();

    if ( mCurBase )
      mCurBase->onMouseDrag ( tmpEvent );
  //}

  tmpEvent.setBase ( mCurBase.get () );

  emitEvent ( tmpEvent, &ui::eventObserver::onMouseDrag );
}

/////////////////////////////////////////////////////////////////////

void manager::onMouseUp ( mouseEvent const& event )
{
  // Protect against some bogus stuff in multitouch-land 
  // where second up is sometimes not reported.  Right
  // now we don't try to synthesize a second up... that's
  // a TODO.
  // UPDATE: We don't want to do this here... filtering out
  // ups if there's no widget selected is widget-specific,
  // not for the global event source.
  
  mouseEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );

  doIsect ( tmpEvent );
  //if ( doIsect ( tmpEvent ) )
  //{
    // Only mouseDown can change the focused widget.
//       mCurBase = tmpEvent.getBase ();

    if ( mCurBase )
      mCurBase->onMouseUp ( tmpEvent );
  //}

  tmpEvent.setBase ( mCurBase.get () );

  emitEvent ( tmpEvent, &ui::eventObserver::onMouseUp );

  mCurBase = 0;
}

/////////////////////////////////////////////////////////////////////

void manager::onMouseHold ( mouseEvent const& event )
{
  mouseEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );

  if ( tmpEvent.getBase () )
  //{
    tmpEvent.getBase ()->onMouseHold ( tmpEvent );
    //tmpEvent.setBase ( mCurBase.get () );
  //}
  
  emitEvent ( tmpEvent, &ui::eventObserver::onMouseHold );
}

void manager::onSingleClick ( mouseEvent const& event )
{
  mouseEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );

  if ( tmpEvent.getBase () )
  //{
    tmpEvent.getBase ()->onSingleClick ( tmpEvent );
    //tmpEvent.setBase ( mCurBase.get () );
  //}
  
  emitEvent ( tmpEvent, &ui::eventObserver::onSingleClick );
}

/////////////////////////////////////////////////////////////////////

void manager::onDoubleClick ( mouseEvent const& event )
{
  mouseEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );

  if ( tmpEvent.getBase () )
  //{
    tmpEvent.getBase ()->onDoubleClick ( tmpEvent );
    //tmpEvent.setBase ( mCurBase.get () );
  //}
  
  emitEvent ( tmpEvent, &ui::eventObserver::onDoubleClick );
}

/////////////////////////////////////////////////////////////////////

void manager::onSingleOnlyClick ( mouseEvent const& event )
{
  mouseEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );

  if ( tmpEvent.getBase () )
  //{
    tmpEvent.getBase ()->onSingleOnlyClick ( tmpEvent );
    //tmpEvent.setBase ( mCurBase.get () );
  //}
  
  emitEvent ( tmpEvent, &ui::eventObserver::onSingleOnlyClick );
}

/////////////////////////////////////////////////////////////////////
// Accel events do not have a widget focus... for now.
// Maybe riff off of what we're doing for text focus.  Maybe not.

void manager::onAccelEvent ( accelEvent const& event )
{
  accelEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );

  emitEvent ( tmpEvent, &ui::eventObserver::onAccelEvent );
  
  // TODO
};

/////////////////////////////////////////////////////////////////////
// Key events need to go to the focused widget... which might be
// limited to a text input widget only.  For now they go to the
// last focused widget always.

void manager::onKeyEvent ( keyEvent const& event )
{
  keyEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );
  
  if ( mTextFocusBase )
  {
    tmpEvent.setBase ( mTextFocusBase.get () );
    
    mTextFocusBase->onKeyEvent ( tmpEvent );
  }

  emitEvent ( tmpEvent, &ui::eventObserver::onKeyEvent );
  // TODO
}
 
/////////////////////////////////////////////////////////////////////
// Only sent when mouse is down/drag/up, so just use mCurBase.

void manager::onSceneEvent ( sceneEvent const& event )
{
  sceneEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );

  if ( mCurBase )
  {
    tmpEvent.setBase ( mCurBase.get () );
    
    mCurBase->onSceneEvent ( tmpEvent );
  }

  emitEvent ( tmpEvent, &ui::eventObserver::onSceneEvent );
  // TODO
}

/////////////////////////////////////////////////////////////////////

void manager::onHeartbeat ( heartbeatEvent const& event )
{
  heartbeatEvent tmpEvent ( event );
  
    // Hmmm... we already set timestamp in calling func.  Is
    // this used to avoid problems with the cooperative mtasking
    // in the app with possible duplicate or old timestamps used
    // during heartbeats? TRICKY?
  tmpEvent.setTimeStamp ( mCurTime );
  
  //if ( mCurBase )
  //{
  //  tmpEvent.setBase ( mCurBase.get () );
  //  
  //  mCurBase->onHeartbeat ( tmpEvent );
  //}
  
  emitEvent ( tmpEvent, &ui::eventObserver::onHeartbeat );
}

/////////////////////////////////////////////////////////////////////

void manager::onFlick ( flickEvent const& event )
{
  flickEvent tmpEvent ( event );
  tmpEvent.setTimeStamp ( mCurTime );

  if ( tmpEvent.getBase () )
  //{    
    tmpEvent.getBase ()->onFlick ( tmpEvent );
    //tmpEvent.setBase ( mCurBase.get () );
  //}
  
  emitEvent ( tmpEvent, &ui::eventObserver::onFlick );
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::setupNearFar ( layer const* pLayer, 
    pni::math::vec3& nearPt,
    pni::math::vec3& farPt )
{
  scene::converter conv;
  scene::nodePath dst ( pLayer->mRoot->getBaseNode () );

  conv.screenToWorld ( nearPt, nearPt, dst, pLayer->mCamPath );
  conv.screenToWorld ( farPt, farPt, dst, pLayer->mCamPath );
}

// Hmmm... so should we even do this for mouseDrag?  That won't 
// change focus... so we don't need a hit test for a new widget.
// But we do like the idea of the sceneEvent and how that enables
// the scene widget to do picking.  Maybe that behavior should move
// into scene widget should synthesize this and we only need to pick
// on mouseDown.  Hmmm... later. TODO.  Do we also need a pick for
// mouseUp to make sure the user let go (at the very last instant)
// outside of the widget?

bool manager::doIsect ( mouseEvent& event )
{
  // make isectDd and related stuff.
  pni::pstd::autoRef< scene::isectTest > pTest = new scene::isectTest;
  pTest->initSegs ( 1 );
  scene::isectDd isect;
  isect.setTest ( pTest.get () );
  
  // Iterate over layers.  Essentially this evaluates which
  // widget in which layer should get current events.  This
  // can be used either as a zsort of a conventional window
  // system or as a way to handle distinct hud and scene layers
  // with distinct zbuffering.
  Layers::const_iterator end = mLayers.end ();
  for ( Layers::const_iterator cur = mLayers.begin ();
      cur != end;
      ++cur )
  {
    if ( ! *cur )
      continue;
  
    // We get our converted near and far in the base node's coordinate
    // frame already... so don't use the base node's xform for
    // isect traversal.
    //pTest->setOps ( ( *cur )->mOps | scene::isectTest::IgnoreFirstXform );
    pTest->setOps ( ( *cur )->mOps | scene::isectTest::IgnoreFirstXform );

    // do screen-to-world conversion
    pni::math::vec3 nearPt ( event.mMousePointers[ 0 ].mXpos,
        event.mMousePointers[ 0 ].mYpos, -1.0f );

    pni::math::vec3 farPt ( event.mMousePointers[ 0 ].mXpos,
        event.mMousePointers[ 0 ].mYpos, 1.0f );

    setupNearFar ( ( *cur ).get (), nearPt, farPt );
    
    // setup collision segment.
    pTest->mSegs[ 0 ].set ( nearPt, farPt );
    
    // do collision
    isect.startGraph ( ( *cur )->mRoot->getBaseNode () );

    if ( pTest->mHits[ 0 ].isValid() )
    {
      // Rescale tval to be in [0,length] scale. (?) 
      // Or maybe allow the app to deal with this?
      // Update: No... don't do this.  mTval makes it
      // back to the app in [0,1].  The app will then use
      // the seg lerp function which expects 't' in the
      // [0,1] range.
//       pTest->mHits[ 0 ].mTval *= pTest->mSegs[ 0 ].getLength ();
    
      // TODO dispatch events to widget(s)
      if ( base* widget = base::findGuiPtr ( pTest->mHits[ 0 ].mNodePath ) )
      {
        // Fill in base pointer in event passed in only if it's not
        // already set.  This is for things like drag that will not
        // change focus from the mouseDown event.
        if ( ! event.getBase () )
          event.setBase ( widget );

        // also dispatch to manager's scene event callback.
        sceneEvent sEvent;
        sEvent.setTimeStamp ( event.getTimeStamp () );
        sEvent.setBase ( widget );
        sEvent.setManager ( this );
        sEvent.mSegs = pTest->mSegs;
        sEvent.mHits = pTest->mHits;
        // TODO Fill in sceneEvent.
        onSceneEvent ( sEvent );
      }

      // Bail out... we're done with this loop with the first valid hit.
      return true;
    }
  }
  
  return false;
}

 
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::manageLayer ( layer* pLayer, size_t which ) 
{
  if ( which >= mLayers.size () )
      mLayers.resize ( which + 1 );

  // TODO: Clear any sticky focus.
  mLayers[ which ] = pLayer;
}

void manager::unmanageLayer ( size_t which )
{
  if ( which < mLayers.size () )
    mLayers[ which ] = 0;
}

void manager::resetLayers ()
{
  // TODO Reset any cached state related to existing layers.
  mLayers.clear ();
}
   
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::update ( TimeType timeStamp )
{
  float lastTimeStamp = mCurTime;
  mCurTime = timeStamp;
  
  float lastFrameStartTime = mThisFrameStartTime;  
  mThisFrameStartTime = mTimer.get ();
  
  mFrameRateAvg.appendSample ( 1.0f / ( mThisFrameStartTime - lastFrameStartTime ) );

  float nonFrameDuration = mThisFrameStartTime - mLastFrameFinishTime;
  mNonFrameTimeAvg.appendSample ( nonFrameDuration );
  

    // TRICKY: Should we really let this one go right here?
  if ( mCurBase && mCurBase->getNumRefs () == 1 )
    mCurBase = 0;
  //else if ( mCurBase )
  //{
    // TODO figure out when and where to send heartbeat event.
    // We do it here for now.
    heartbeatEvent tmpEvent;
    tmpEvent.setTimeStamp ( timeStamp );
    tmpEvent.setBase ( mCurBase.get () );
    tmpEvent.setManager ( this );
    onHeartbeat ( tmpEvent );
  //}

  // TODO timing-based event evaluation like missing 'ups' and 
  // gestures, etc.

    // Draw layers, clearing color only for the first...
    // The app is responsible for setting the right buffer clear
    // values on the camera(s).

//printf ( "ui::manger::update\n" );
  Layers::const_reverse_iterator end = mLayers.rend ();
  for ( Layers::const_reverse_iterator cur = mLayers.rbegin ();
      cur != end;
      ++cur )
  {
    if ( ! *cur )
      continue;
      
    scene::graphDd* pDd = ( *cur )->mDd;

    pDd->setTravMask ( ( *cur )->mTravMask );
    pDd->setSinkPath ( ( *cur)->mCamPath );
    pDd->setTimeStamp ( timeStamp );
    pDd->setLastTimeStamp ( lastTimeStamp );
    pDd->startGraph ( ( *cur )->mRoot->getBaseNode () );

    pDd->setSinkPath ( scene::nodePath () );
    
    if ( scene::graphDd* pSndDd = ( *cur )->mSndDd )
    {
      pSndDd->setTravMask ( ( *cur )->mSndTravMask );
      pSndDd->setSinkPath ( ( *cur)->mSndListenerPath );
      pSndDd->setTimeStamp ( timeStamp );
      pSndDd->setLastTimeStamp ( lastTimeStamp );
      pSndDd->startGraph ( ( *cur )->mSndRoot.get () );
    }
  }
  
  mLastFrameFinishTime = mTimer.get ();
  
  mFrameTimeAvg.appendSample ( mLastFrameFinishTime - mThisFrameStartTime );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

    //pni::pstd::avg< TimeType > mFrameRate;
    //pni::pstd::avg< TimeType > mFrameTime;
    //pni::pstd::avg< TimeType > mNonFrameTime;


float manager::getFrameRateAverage () const
{
  return mFrameRateAvg.getCurAverage ();
}

float manager::getFrameDurationAverage () const
{
  return mFrameTimeAvg.getCurAverage ();
}

float manager::getNonFrameDurationAverage () const
{
  return mNonFrameTimeAvg.getCurAverage ();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void manager::clearCaches ()
{
  mCurBase = 0;
  mTextFocusBase = 0;
}

/////////////////////////////////////////////////////////////////////

    //Layers mLayers;
    //BaseRef mCurBase;
    //BaseRef mTextFocusBase;

void manager::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  pni::pstd::dbgRefCount::collectVecRefs ( mLayers, refs );

  refs.push_back ( mCurBase.get () );
  refs.push_back ( mTextFocusBase.get () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace ui 


