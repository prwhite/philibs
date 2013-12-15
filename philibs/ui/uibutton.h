/////////////////////////////////////////////////////////////////////
//
//    class: button
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuibutton_h
#define uiuibutton_h

/////////////////////////////////////////////////////////////////////

#include "uibase.h"

#include "uievents.h"

/////////////////////////////////////////////////////////////////////

namespace ui {
  
/////////////////////////////////////////////////////////////////////

class button :
  public base
{
  public:
    button();
     //virtual ~button();
     //button(button const& rhs);
     //button& operator=(button const& rhs);
     //bool operator==(button const& rhs) const;

    struct buttonEvent :
      public event
    {
        
    };
    
    struct buttonObserver
    {
      virtual void onButtonPressed ( buttonEvent const& event ) {}
    };
    
    void setButtonObserver ( buttonObserver* pObserver ) { mButtonObserver = pObserver; }
    buttonObserver* getButtonObserver () const { return mButtonObserver; }

  protected:
    buttonObserver* mButtonObserver;
    
  private:
    

  // interface from base
  public:
    
  protected:
//     virtual void onMouseDown ( mouseEvent const& event );
//     virtual void onMouseDrag ( mouseEvent const& event );
    virtual void onMouseUp ( mouseEvent const& event );
//     virtual void onAccelEvent ( accelEvent const& event );
//     virtual void onKeyEvent ( keyEvent const& event );
//     virtual void onSceneEvent ( sceneEvent const& event );
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuibutton_h


