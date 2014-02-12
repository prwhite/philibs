/////////////////////////////////////////////////////////////////////
//
//    class: manager
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuimanager_h
#define uiuimanager_h

/////////////////////////////////////////////////////////////////////

#include "uievents.h"
#include "uieventobserver.h"
#include "uibase.h"
#include "uiframe.h"
#include "scenenodepath.h"
#include "scenegraphdd.h"

#include "pnirefcount.h"
#include "pniautoref.h"

#include "pnitimer.h"
#include "pniavg.h"

#include <vector>
#include <set>

/////////////////////////////////////////////////////////////////////

namespace ui {
  struct mouseEvent;
  struct accelEvent;
  struct keyEvent;
  struct sceneEvent;
  struct flickEvent;
  class manager;
  
  struct flickObserver;
  struct clickObserver;

/////////////////////////////////////////////////////////////////////

class layer :
  public pni::pstd::refCount
{
  public:
    layer () : 
      mOps ( scene::isectTest::Default ),
      mDd ( 0 ),
      mSndDd ( 0 ),
      mTravMask ( ~0x00 ),
      mSndTravMask ( ~0x00 ),
      mUiMgr ( 0 )
        {}

    ~layer ();

    virtual void setUiManager ( manager* pUiMgr );
    manager* getUiManager () const { return mUiMgr; }
    
    typedef pni::pstd::autoRef< frame > FrameRef;
    
    scene::graphDd* mDd;
    scene::graphDd* mSndDd;
    scene::nodePath mCamPath;
    scene::nodePath mSndListenerPath;
    unsigned int mTravMask;
    unsigned int mSndTravMask;
    FrameRef mRoot;
    pni::pstd::autoRef< scene::node > mSndRoot;
    unsigned int mOps;
  
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
  
  private:
    manager* mUiMgr;
    
    // isectTest::Ops for picking only geom bounds?
};

/////////////////////////////////////////////////////////////////////

class manager :
  public ui::eventObserver,
  public pni::pstd::refCount
{
  public:
    manager();
    //manager(manager const& rhs);
    //manager& operator=(manager const& rhs);
    //bool operator==(manager const& rhs) const;
   
      ///////
      // Overrides for eventObserver... app glue code should 
      // call this with low-level events.
    virtual void onMouseDown ( mouseEvent const& event );
    virtual void onMouseDrag ( mouseEvent const& event );
    virtual void onMouseUp ( mouseEvent const& event );
    virtual void onMouseHold ( mouseEvent const& event );
    virtual void onSingleClick ( mouseEvent const& event );
    virtual void onDoubleClick ( mouseEvent const& event );
    virtual void onSingleOnlyClick ( mouseEvent const& event );
    virtual void onAccelEvent ( accelEvent const& event );
    virtual void onKeyEvent ( keyEvent const& event );
    virtual void onSceneEvent ( sceneEvent const& event );
    virtual void onHeartbeat ( heartbeatEvent const& event );
    virtual void onFlick ( flickEvent const& event );
    
      ///////
    typedef pni::pstd::autoRef< layer > LayerRef;
    typedef std::vector< LayerRef > Layers;
    
    void manageLayer ( layer* pLayer, size_t which );
    void unmanageLayer ( size_t which );
    Layers& getLayers () { return mLayers; }
    void resetLayers ();

      ///////
      // Call this every frame... manager does some time-based
      // processing to recognize gestures, etc.
    void update ( TimeType timeStamp );
    
    TimeType getCurTime () { return mCurTime; }
    
    float getFrameRateAverage () const;
    float getFrameDurationAverage () const;
    float getNonFrameDurationAverage () const;
        
      ///////
      // Setup global event observer... events "T" off of the
      // event manager to this observer... and also continue on
      // through to individual widgets.
//     void setEventObserver ( eventObserver* pObserver ) { mObserver = pObserver; }
//     eventObserver* getEventObserver () const { return mObserver; }

      // Global event observers.
    typedef std::set< eventObserver* > Observers;
    void addEventObserver ( eventObserver* pObserver );
    void remEventObserver ( eventObserver* pObserver );
    Observers& getEventObservers () { return mObservers; }
    Observers const& getEventObservers () const { return mObservers; }
    
      // Cleanup ephemeral data.
    void clearCaches ();
    
  protected:
    virtual ~manager();
    
    template< class Event, class Func >
    void emitEvent ( Event const& anEvent, Func func );
    
    void setupNearFar ( layer const* pLayer, 
        pni::math::vec3& nearPt,
        pni::math::vec3& farPt );
    bool doIsect ( mouseEvent& event );
    
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
    
  private:
    typedef pni::pstd::autoRef< base > BaseRef;
  
    Layers mLayers;
    BaseRef mCurBase;
    BaseRef mTextFocusBase;
    flickObserver* mFlickObserver;
    clickObserver* mClickObserver;
    Observers mObservers;
    TimeType mCurTime;
    TimeType mThisFrameStartTime;
    TimeType mLastFrameFinishTime;
    
    pni::pstd::timer mTimer;
    pni::pstd::avg< TimeType > mFrameRateAvg;
    pni::pstd::avg< TimeType > mFrameTimeAvg;
    pni::pstd::avg< TimeType > mNonFrameTimeAvg;
};

/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuimanager_h


