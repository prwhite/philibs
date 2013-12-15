/////////////////////////////////////////////////////////////////////
//
//    file: uibase.cpp
//
/////////////////////////////////////////////////////////////////////

#include "uibase.h"

#include "scenegroup.h"

#include "pnirefcountdbg.h"

#include <iostream>
#include <cassert>

/////////////////////////////////////////////////////////////////////

namespace ui {
  
/////////////////////////////////////////////////////////////////////

base::base() :
  mObserver ( 0 ),
  mVisibility ( true ),
  mEnable ( true )
{

}

base::~base()
{

}

// base::base(base const& rhs)
// {
//   
// }
// 
// base& base::operator=(base const& rhs)
// {
//   
//   return *this;
// }
// 
// bool base::operator==(base const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void base::initBaseNode ( scene::node* pNode ) 
{ 
  mRootNode = pNode;
  mBaseNode = pNode;  // Aliased... trouble?
  setupBackPtr ( mBaseNode.get (), this );
  //setEnable ( mEnable );
}

/////////////////////////////////////////////////////////////////////

void base::initRootNode ( scene::node* pNode ) 
{ 
  mBaseNode = new scene::group;
  setupBackPtr ( mBaseNode.get (), this );

  mRootNode = pNode;
  mBaseNode->addChild ( pNode );
  //setEnable ( mEnable );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void base::setVisibility ( bool vis )
{
  mVisibility = vis;
  if ( vis )
  {
    mBaseNode->setTravMask ( scene::Draw, ~0x00 );
    mBaseNode->setTravMask ( scene::Isect, ~0x00 );    
  }
  else
  {
    mBaseNode->setTravMask ( scene::Draw, 0x00 );
    mBaseNode->setTravMask ( scene::Isect, 0x00 );
  }
}

void base::setEnable ( bool val )
{
  mEnable = val;

  assert ( mBaseNode );
  
  if ( mEnable )
    activateAction ( "enable" );
  else
    activateAction ( "disable" );
  
  if ( val )
    mBaseNode->setTravMask ( scene::Isect, ~0x00 );    
  else
    mBaseNode->setTravMask ( scene::Isect, 0x00 );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void base::addAction ( std::string const& name, action* pAction )
{
  mActions[ name ] = pAction;
  
  pAction->init ( this );
}

/////////////////////////////////////////////////////////////////////

void base::remAction ( std::string const& str )
{
  Actions::iterator found = mActions.find ( str );
  
  if ( found != mActions.end () )
    mActions.erase ( found );
}

/////////////////////////////////////////////////////////////////////

void base::activateAction ( std::string const& str )
{
  Actions::iterator found = mActions.find ( str );
  
  if ( found != mActions.end () )
    activateAction ( found->second.get () );
}

/////////////////////////////////////////////////////////////////////

void base::activateAction ( action* pAction )
{
  if ( mCurAction )
    mCurAction->finalize ( this );
  
  mCurAction = pAction;
  
  if ( mCurAction )
    mCurAction->execute ( this );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

scene::matrix4& base::matrixOp () 
{ 
  return mBaseNode->matrixOp ();
}

scene::matrix4& base::getMatrix ()
{
  return mBaseNode->getMatrix ();
}

scene::matrix4 const& base::getMatrix () const
{
  return mBaseNode->getMatrix ();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

scene::box3 const& base::getBounds () const
{
    return mBaseNode->getBounds ();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// backPtr for nodes to point back to gui instances.

class backPtr :
  public scene::attr
{
  public:
    backPtr ( base* pBase ) { mBaseNode = pBase; }
  
    virtual attr* dup () const { return 0; }
    
    base* mBaseNode;
};

///////

void base::setupBackPtr ( scene::node* pNode, base* pBase )
{
  pNode->setTravData ( scene::UI, new backPtr ( pBase ) );
}

base* base::findGuiPtr ( scene::nodePath& path )
{
  typedef scene::nodePath::Nodes::iterator Iter;
  
  Iter end = path.getNodes ().end ();
  for ( Iter cur = path.getNodes ().begin ();
      cur != end;
      ++cur )
  {
    scene::node* pNode = *cur;
    if ( void* vptr = pNode->getTravData ( scene::UI ) )
    {
      backPtr* ptr = static_cast< backPtr* > ( vptr );
      return ptr->mBaseNode;
    }
  }

  return 0;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void base::onMouseDown ( mouseEvent const& event )
{
  if ( mObserver )
    mObserver->onMouseDown ( event );
}

void base::onMouseDrag ( mouseEvent const& event )
{
  if ( mObserver )
    mObserver->onMouseDrag ( event );
}

void base::onMouseUp ( mouseEvent const& event )
{
  if ( mObserver )
    mObserver->onMouseUp ( event );
}

void base::onMouseHold ( mouseEvent const& event )
{
  if ( mObserver )
    mObserver->onMouseHold ( event );
}

void base::onSingleClick ( mouseEvent const& event )
{
  if ( mObserver )
    mObserver->onSingleClick ( event );
}

void base::onDoubleClick ( mouseEvent const& event )
{
  if ( mObserver )
    mObserver->onDoubleClick ( event );
}

void base::onSingleOnlyClick ( mouseEvent const& event )
{
  if ( mObserver )
    mObserver->onSingleOnlyClick ( event );
}

void base::onAccelEvent ( accelEvent const& event )
{
  if ( mObserver )
    mObserver->onAccelEvent ( event );
}

void base::onKeyEvent ( keyEvent const& event )
{
  if ( mObserver )
    mObserver->onKeyEvent ( event );
}

void base::onSceneEvent ( sceneEvent const& event )
{
  if ( mObserver )
    mObserver->onSceneEvent ( event );
}

void base::onHeartbeat ( heartbeatEvent const& event )
{
  if ( mObserver )
    mObserver->onHeartbeat ( event );
}

void base::onFlick ( ui::flickEvent const& event )
{
  if ( mObserver )
    mObserver->onFlick ( event );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void base::onDoDbg ()
{
  std::ostream* pStr = pni::pstd::dbgRefCount::getStr ();
  
  *pStr << "   mName = " << mName;
}

    //NodeRef mBaseNode;
    //NodeRef mRootNode;
    //Actions mActions;
    //ActionRef mCurAction;

void base::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  refs.push_back ( mBaseNode.get () );
  refs.push_back ( mRootNode.get () );
  pni::pstd::dbgRefCount::collectMapSecondRefs ( mActions, refs );
  refs.push_back ( mCurAction.get () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace ui 


