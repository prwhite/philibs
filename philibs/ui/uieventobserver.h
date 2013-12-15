/////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuieventobserver_h
#define uiuieventobserver_h

/////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif

/////////////////////////////////////////////////////////////////////

namespace ui {

  struct mouseEvent;
  struct accelEvent;
  struct keyEvent;
  struct sceneEvent;
  struct heartbeatEvent;
  struct flickEvent;

/////////////////////////////////////////////////////////////////////

class eventObserver
{
  public:
//     events();
    virtual ~eventObserver();
//     events(events const& rhs);
//     events& operator=(events const& rhs);
//     bool operator==(events const& rhs) const;

    virtual void onMouseDown ( mouseEvent const& event ) {}
    virtual void onMouseDrag ( mouseEvent const& event ) {}
    virtual void onMouseUp ( mouseEvent const& event ) {}
    virtual void onMouseHold ( mouseEvent const& event ) {}
    virtual void onSingleClick ( mouseEvent const& event ) {}
    virtual void onDoubleClick ( mouseEvent const& event ) {}
    virtual void onSingleOnlyClick ( mouseEvent const& event ) {}
    virtual void onAccelEvent ( accelEvent const& event ) {}
    virtual void onKeyEvent ( keyEvent const& event ) {}
    virtual void onSceneEvent ( sceneEvent const& event ) {}
    virtual void onHeartbeat ( heartbeatEvent const& event ) {}
    virtual void onFlick ( flickEvent const& event ) {}
    
  protected:
    
  private:
    

};

/////////////////////////////////////////////////////////////////////

template< class Selector >
class eventObserverShim
{
    class localObserver :
      public eventObserver
        {
            eventObserverShim* mShim;
            
          public:
            localObserver ( eventObserverShim* pShim )
              : mShim ( pShim ) {}
            
            virtual void onMouseDown ( mouseEvent const& event )
                { mShim->onMouseDownShim ( event, Selector () ); }
            virtual void onMouseDrag ( mouseEvent const& event )
                { mShim->onMouseDragShim ( event, Selector () ); }
            virtual void onMouseUp ( mouseEvent const& event )
                { mShim->onMouseUpShim ( event, Selector () ); }
            virtual void onMouseHold ( mouseEvent const& event )
                { mShim->onMouseHoldShim ( event, Selector () ); }
            virtual void onSingleClick ( mouseEvent const& event )
                { mShim->onSingleClickShim ( event, Selector () ); }
            virtual void onDoubleClick ( mouseEvent const& event )
                { mShim->onDoubleClickShim ( event, Selector () ); }
            virtual void onSingleOnlyClick ( mouseEvent const& event )
                { mShim->onSingleOnlyClickShim ( event, Selector () ); }
            virtual void onAccelEvent ( accelEvent const& event )
                { mShim->onAccelEventShim ( event, Selector () ); }
            virtual void onKeyEvent ( keyEvent const& event )
                { mShim->onKeyEventShim ( event, Selector () ); }
            virtual void onSceneEvent ( sceneEvent const& event )
                { mShim->onSceneEventShim ( event, Selector () ); }
            virtual void onHeartbeat ( heartbeatEvent const& event )
                { mShim->onHeartbeatShim ( event, Selector () ); }
            virtual void onFlick ( flickEvent const& event )
                { mShim->onFlickShim ( event, Selector () ); }
        };
        
    localObserver mLocalObserver;
    

  public:
    
    eventObserverShim () : mLocalObserver ( this ) {}
  
    eventObserver* getShimObserver ( Selector const& ) { return &mLocalObserver; }

    virtual void onMouseDownShim ( mouseEvent const& event, Selector const& sel ) {}
    virtual void onMouseDragShim ( mouseEvent const& event, Selector const& sel ) {}
    virtual void onMouseUpShim ( mouseEvent const& event, Selector const& sel ) {}
    virtual void onMouseHoldShim ( mouseEvent const& event, Selector const& sel ) {}
    virtual void onSingleClickShim ( mouseEvent const& event, Selector const& sel ) {}
    virtual void onDoubleClickShim ( mouseEvent const& event, Selector const& sel ) {}
    virtual void onSingleOnlyClickShim ( mouseEvent const& event, Selector const& sel ) {}
    virtual void onAccelEventShim ( accelEvent const& event, Selector const& sel ) {}
    virtual void onKeyEventShim ( keyEvent const& event, Selector const& sel ) {}
    virtual void onSceneEventShim ( sceneEvent const& event, Selector const& sel ) {}
    virtual void onHeartbeatShim ( heartbeatEvent const& event, Selector const& sel ) {}
    virtual void onFlickShim ( flickEvent const& event, Selector const& sel ) {}

    

};


/////////////////////////////////////////////////////////////////////

} // end namespace ui

/////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning( pop )
#endif


#endif // uiuieventobserver_h
