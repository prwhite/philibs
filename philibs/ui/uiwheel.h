/////////////////////////////////////////////////////////////////////
//
//    class: wheel
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuiwheel_h
#define uiuiwheel_h

/////////////////////////////////////////////////////////////////////

#include "uibase.h"

#include "uievents.h"
#include "uiwheelcore.h"

#include "pnivec2.h"
#include "pnivec3.h"

  // TEMP
#include "pnilog.h"

/////////////////////////////////////////////////////////////////////

namespace ui {

class wheelShimSelector {};
  
/////////////////////////////////////////////////////////////////////

class wheel :
  public base,
  protected eventObserverShim< wheelShimSelector >,
  public ui::wheelCore
{
  public:
    wheel();
    virtual ~wheel();
    //wheel(wheel const& rhs);
    //wheel& operator=(wheel const& rhs);
    //bool operator==(wheel const& rhs) const;
    
    void initUiManager ( ui::manager* pMgr ) { mUiMgr = pMgr; }
    virtual void initBaseNode ( scene::node* pNode );
    
      //////////////////
    struct wheelEvent :
      public ui::event
    {
      
    };
    
    struct wheelObserver 
    {
      virtual void onDragStart ( wheelEvent const& anEvent ) {}
      virtual void onSelectionChanged ( wheelEvent const& anEvent ) {}
      virtual void onWheelScroll ( wheelEvent const& anEvent ) {}
    };
    
    void setWheelObserver ( wheelObserver* pObs ) { mObserver = pObs; }
    wheelObserver* getWheelObserver () const { return mObserver; }
    
      //////////////////
    void initGrooveNode ( scene::node* pNode );
    
    void setQuant ( float val ) { setItemSize ( val ); }
    float getQuant () const { return getItemSize (); }
    
    enum Orientation { Xaxis, Yaxis };
    void setOrientation ( Orientation which ) { mMouseDimension = which; };
    Orientation getOrientation () const { return ( Orientation ) mMouseDimension.get (); }
    
      // Vis orientation tells the widget which axis should be used
      // to update the visualization of the wheel.    
    void setVisOrientation ( Orientation orient ) { mVisOrient = orient; }
    Orientation getVisOrientation () const { return mVisOrient; }
    
      // Vis quant is the length of the repeating pattern used
      // for the wheel visualization (if any).  If there is no
      // repeating pattern, set this to a very high number.
      // The default is a very high number.
      // UNTESTED: Using a very high number/trying to not have a repeat.
    void setVisQuant ( float quant ) { mVisQuant = quant; }
    float getVisQuant () const { return mVisQuant; }
        
      // Distance effect sets how much movement is modulated
      // by the perpendicular distance of the finger from the 
      // wheel.  Falloff is linear.
      // Default is 0.0f (no effect).
      // Range is [0.0f],[1.0f,dist to kill all input]
    void setDistanceEffect ( float val ) { mDistEff = val; }
    float getDistanceEffect () const { return mDistEff; }
    
    void setCurMod ( float val ) { mModVal = val; }
    float getCurMod () const { return mModVal; }

    using wheelCore::setCurVal;
    using wheelCore::getCurVal;
    
    using wheelCore::setCurModVal;
    using wheelCore::getCurModVal;
    
    using wheelCore::setMinMax;
    using wheelCore::setModMinMax;
    using wheelCore::clearMinMax;
    

  protected:

      // From ui::wheelCore.
    virtual void onSetDirty ( bool val ) { /*mDirty = val;*/ }
    virtual bool isGoodMouseDown ( ui::mouseEvent const& mevent ) { return true; }
    virtual void onDragStart ( ui::wheelCore* pWheel, ui::wheelCore::event const& wevent );
    virtual void onSelectionChanged ( ui::wheelCore* pWheel, ui::wheelCore::event const& wevent );
    virtual void onWheelScroll ( ui::wheelCore* pWheel, ui::wheelCore::event const& wevent );

    void updateTrans ();
  
    wheelObserver* mObserver;
    NodeRef mGrooves;
    
    pni::pstd::autoRef< ui::manager > mUiMgr;
    pni::math::vec3 mTransOrig;
    pni::math::vec2 mLastPoint;

    float mVisQuant;  // Wraparound for visualization of wheel.
    float mDistEff;   // Modulation of movement due to dist from wheel.
    Orientation mVisOrient;
    
  private:
    

  // interface from base
  public:

    virtual void onMouseDown ( mouseEvent const& event );
    virtual void onMouseDrag ( mouseEvent const& event );
    virtual void onMouseUp ( mouseEvent const& event );
//     virtual void onAccelEvent ( accelEvent const& event );
//     virtual void onKeyEvent ( keyEvent const& event );
//     virtual void onSceneEvent ( sceneEvent const& event );
    virtual void onHeartbeatShim ( heartbeatEvent const& event, wheelShimSelector const& );
    
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );

};

/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuiwheel_h


