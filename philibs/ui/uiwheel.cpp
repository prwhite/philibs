/////////////////////////////////////////////////////////////////////
//
//    file: uiwheel.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE

#include "uiwheel.h"

// Temporary:

#include <iostream>
#include "pnimathstream.h"
#include "uimanager.h"

/////////////////////////////////////////////////////////////////////

namespace ui {

/////////////////////////////////////////////////////////////////////

wheel::wheel() :
  mObserver ( 0 ),
  mVisQuant ( pni::math::Trait::maxVal ), // There's no good default for this.
  mDistEff ( 0.0f ),
  mVisOrient ( Xaxis )
{
  
}

wheel::~wheel()
{
  mUiMgr->remEventObserver ( getShimObserver ( wheelShimSelector () ) );
}

// wheel::wheel(wheel const& rhs)
// {
//   
// }
// 
// wheel& wheel::operator=(wheel const& rhs)
// {
//   
//   return *this;
// }
// 
// bool wheel::operator==(wheel const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from base

void wheel::initBaseNode ( scene::node* pNode )
{
  base::initBaseNode ( pNode );
  
  mUiMgr->addEventObserver ( getShimObserver ( wheelShimSelector () ) );

  //setItemSize ( 60.0f );
  mVelDamp = -1.0f;
  mVelMin = 60.0f;
  //mMouseDimension = 0;
  //setMinMax ( 0.0f, getItemSize () * ( mWeapons.size () - 1 ) );
}

/////////////////////////////////////////////////////////////////////

void wheel::initGrooveNode ( scene::node* pNode )
{
  mGrooves = pNode;
  pNode->getMatrix ().getTrans ( mTransOrig );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void wheel::onMouseDown ( mouseEvent const& event )
{
  base::onMouseDown ( event );

  onWheelDown ( event );
}

/////////////////////////////////////////////////////////////////////

void wheel::onMouseUp ( mouseEvent const& event )
{
  base::onMouseUp ( event );
  
  onWheelUp ( event );
}

/////////////////////////////////////////////////////////////////////

void wheel::onMouseDrag ( mouseEvent const& event )
{
  base::onMouseDrag ( event );
  
  onWheelDrag ( event );
}

/////////////////////////////////////////////////////////////////////

void wheel::onHeartbeatShim ( heartbeatEvent const& event, 
    wheelShimSelector const& )
{
  onWheelHeartbeat ( event );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void wheel::onDragStart ( ui::wheelCore* pWheel, ui::wheelCore::event const& event )
{
  if ( mObserver )
  {
    wheel::wheelEvent wEvent;
    wEvent.setTimeStamp ( event.mTimeStamp );
    wEvent.setBase ( this );
    wEvent.setManager ( mUiMgr.get () );
    mObserver->onDragStart ( wEvent );
  }
}

/////////////////////////////////////////////////////////////////////

void wheel::onSelectionChanged ( ui::wheelCore* pWheel, ui::wheelCore::event const& event )
{
  if ( mObserver )
  {
    wheel::wheelEvent wEvent;
    wEvent.setTimeStamp ( event.mTimeStamp );
    wEvent.setBase ( this );
    wEvent.setManager ( mUiMgr.get () );
    mObserver->onSelectionChanged ( wEvent );
  }
}

/////////////////////////////////////////////////////////////////////

void wheel::onWheelScroll ( ui::wheelCore* pWheel, ui::wheelCore::event const& event )
{
  updateTrans ();

  if ( mObserver )
  {
    wheel::wheelEvent wEvent;
    wEvent.setTimeStamp ( event.mTimeStamp );
    wEvent.setBase ( this );
    wEvent.setManager ( mUiMgr.get () );
    mObserver->onWheelScroll ( wEvent );
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void wheel::updateTrans ()
{
  const float ModVal = mVisQuant;

  float fdiff = -fmodf ( getCurVal (), ModVal );
  if ( fdiff < 0.0f )
    fdiff += ModVal;
  
  pni::math::vec3 tmp ( mTransOrig );
  tmp[ mVisOrient ] += fdiff;

// std::cout << " start = " << mStartPoint << " out = " << tmp 
//     << " fdiff = " << fdiff << std::endl;

  mGrooves->matrixOp ().setTrans ( tmp );
  
  //if ( mObserver )
  //  mObserver->onWheelScroll ( wheelEvent  () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void wheel::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  refs.push_back ( mUiMgr.get () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////


