/////////////////////////////////////////////////////////////////////
//
//    file: uibutton.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIPSTDLOGDISABLE

#include "pnilog.h"

#include "uibutton.h"

/////////////////////////////////////////////////////////////////////

namespace ui {
  
/////////////////////////////////////////////////////////////////////

button::button() :  
  mButtonObserver ( 0 )
{
  
}

// button::~button()
// {
//   
// }
// 
// button::button(button const& rhs)
// {
//   
// }
// 
// button& button::operator=(button const& rhs)
// {
//   
//   return *this;
// }
// 
// bool button::operator==(button const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from base

// void button::onMouseDown ( mouseEvent const& event )
// {
//   // TODO
// }

// void button::onMouseDrag ( mouseEvent const& event )
// {
//   // TODO
// }

void button::onMouseUp ( mouseEvent const& event )
{
  base::onMouseUp ( event );
PNIPSTDLOG
  if ( mEnable && event.getBase () == this )
  {
PNIPSTDLOG
      // Send button press event.
    if ( mButtonObserver )
    {
      buttonEvent bevent;
      bevent.setTimeStamp ( event.getTimeStamp () );
      bevent.setBase ( this );
      
      mButtonObserver->onButtonPressed ( bevent );
    }
  }
}

// void button::onAccelEvent ( accelEvent const& event )
// {
//   // TODO
// }
// 
// void button::onKeyEvent ( keyEvent const& event )
// {
//   // TODO
// }
// 
// void button::onSceneEvent ( sceneEvent const& event )
// {
//   // TODO
// }

/////////////////////////////////////////////////////////////////////


} // end of namespace ui 


