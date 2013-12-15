/////////////////////////////////////////////////////////////////////
//
//    class: base
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuibase_h
#define uiuibase_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "pniautoref.h"

#include "uieventobserver.h"

#include "pnimatrix4.h"

#include "scenenode.h"
#include "scenetypes.h"
#include "scenenodepath.h"

#include <map>

/////////////////////////////////////////////////////////////////////

namespace ui {
   class base;
  
/////////////////////////////////////////////////////////////////////

class action :
  public pni::pstd::refCount
{
  public:
  
    virtual ~action () {}
  
    virtual void init ( base* pBase ) {}
    virtual void execute ( base* pBase ) {}
    virtual void finalize ( base* pBase ) {}
    
  protected:

    void collectRefs ( pni::pstd::refCount::Refs& refs ) {}

    std::string mName;
};

/////////////////////////////////////////////////////////////////////

class base :
  public pni::pstd::refCount,
  public ui::eventObserver
{
    friend class frame;

  public:
    base();
//     base(base const& rhs);
//     base& operator=(base const& rhs);
//     bool operator==(base const& rhs) const;
    

    enum States {
      Init,   // Special state upon construction.
      Normal,
      Select,
      Unselect,
      Disable,
      States
    };

    void setName ( std::string const& str ) { mName = str; }
    std::string const& getName () const { return mName; }

      ///////
      // Setting root/base node... can only call these once...
      // cannot change the note.

      // Here's a view of the internal structure of a widget:
      // For initBaseNode:
      //  baseNode->thisWidgetGeom...
      //         |->childBaseNode...
      // For initRootNode:
      //  baseNode->rootNode->thisWidgetGeom...
      //          |->childBaseNode...
      // For leaf widgets, it is not necessary for the baseNode
      // to be a group.

      // Will not create a new base node... will use node passed in
      // as base node and root node... only use for mega-GUIs that
      // are all pre-modeled in place (monolithic).
    virtual void initBaseNode ( scene::node* pNode );
    scene::node* getBaseNode () const { return mBaseNode.get (); }
    
      // Will manufacture a base node... only use for constructing
      // GUIs from scratch.
    virtual void initRootNode ( scene::node* pNode );
    scene::node* getRootNode () const { return mRootNode.get (); }

      // Determine if this node was initialized via setBaseNode
      // for use in mega-gui style construction.
    bool isMegaGui () { return ( mBaseNode == mRootNode ); }

      ///////
    void addAction ( std::string const& name, action* pAction );
    void remAction ( std::string const& str );
    void activateAction ( std::string const& str );

      ///////
    virtual void setVisibility ( bool vis );
    bool getVisibility () const { return mVisibility; }
    
    virtual void setEnable ( bool val );
    bool getEnable () const { return mEnable; }
    
    virtual scene::matrix4& matrixOp ();
    scene::matrix4& getMatrix ();
    scene::matrix4 const& getMatrix () const;
    
    scene::box3 const& getBounds () const;

      // Public typedefs... here?
    typedef pni::pstd::autoRef< base > BaseRef;

    // TODO Need non-const parent interface too?
    base* getParent () { return mParent; }

    ///////
    // Manage observer... only one for now.
    void setEventObserver ( eventObserver* pObserver ) { mObserver = pObserver; }
    eventObserver* getEventObserver () const { return mObserver; }


    ///////
    static base* findGuiPtr ( scene::nodePath& path );
    
  protected:
    virtual ~base();

    void setupBackPtr ( scene::node* pNode, base* pBase );
    
    void activateAction ( action* pAction );
  
    typedef pni::pstd::autoRef< scene::node > NodeRef;

    typedef std::map< std::string, NodeRef > NodeMap;
    
    typedef pni::pstd::autoRef< action > ActionRef;
    typedef std::map< std::string, ActionRef > Actions;
    
    base* mParent;
    NodeRef mBaseNode;
    NodeRef mRootNode;
    Actions mActions;
    ActionRef mCurAction;
    std::string mName;
    eventObserver* mObserver;
    bool mVisibility;
    bool mEnable;
    
  private:

  /////// Inherited from ui::eventObserver
  public:    
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
    virtual void onFlick ( ui::flickEvent const& event );

    // From refCount
  protected:
    virtual void onDoDbg ();
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
};

/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuibase_h


