/////////////////////////////////////////////////////////////////////
//
//    class: ddOglList
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneddogllist_h
#define scenesceneddogllist_h

/////////////////////////////////////////////////////////////////////

#include "scenegraphdd.h"
#include "scenestatedd.h"

#include <cstring>
#include <stack>
#include <vector>

#include "scenestate.h"
#include "pnimatrix4.h"
#include "pnimatstack.h"
#include "pnibox3.h"

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

class ddOglList :
  public scene::graphDd, 
  public scene::stateDd
{
  public:
    ddOglList();
    virtual ~ddOglList();
//     ddOglList(ddOglList const& rhs);
//     ddOglList& operator=(ddOglList const& rhs);
//     bool operator==(ddOglList const& rhs) const;
    

    ///// Types introduced for this class.
    // State stack.
    // For efficiency, states are not refcounted during
    // the render pass.
//    typedef pni::pstd::autoRef< state > StateRef;
    typedef state* StateRef;
    typedef state const* StateConstRef;

    struct stateSet
    {
      stateSet () 
          {
            memset ( mStates, 0x00, sizeof ( StateConstRef[ state::StateCount ] ) );
          }
          
      StateConstRef& operator [] ( unsigned int val ) { return mStates[ val ]; }

      StateConstRef mStates[ state::StateCount ];
    };
    
    typedef std::vector< stateSet > StdStateStack;    
    
    // Matrix stack.
    typedef pni::math::matStack< pni::math::matrix4 > MatStack;
    
    // Bounding box stack.
    typedef std::vector< pni::math::box3 > BoundsStack;

    struct renderItem
    {
      // Purposefully does no initialization.  Users of
      // this class must set all members or risk grave danger.
      renderItem () {}
    
      pni::math::matrix4 mMatrix;
      stateSet mStateSet;       // Really?  The whole set?
      // We do not ref-count things during the rendering pass.
      // pni::pstd::autoRef< node > mNode;
      node const* mNode;
      float distSqr;
    };
    
    typedef std::vector< renderItem > RenderList;
    
    ///// Concrete methods.
    
    // Manage render list items.
    // Alloc one renderItem and return it for use.
    renderItem& alloc ();
    void resetRenderItems ();
    
    // Do the actual rendering.
    void startList ();  // More args??? PRW
    
    void setSinkPath ( nodePath const& cPath ) { mSinkPath = cPath; }
    
    virtual void bindTextures ( node const* pNode );
    
  protected:
    
  private:
    typedef unsigned int GlEnum;
    stateSet mCurStates;
    RenderList mRenderList;
    //CPVRTglesExt mPvr;
    nodePath mSinkPath;
    unsigned int mCurStateId;
    GlEnum mCurLightUnit;
    
    void resetCurState ();
    void execStates ( stateSet const& sSet );
    void execCamera ();
    void execLightPath ( nodePath const& lpath );

  // interface from scenegraphdd and scenestatedd
  public:
    
    // To start a graph traversal.
    virtual void startGraph ( node const* pNode );
    virtual void startStates ( node const* pNode );
    virtual void setDefaultState ( state const* pState, state::Id id );
    
  protected:

    virtual void dispatch ( camera const* pNode );
    virtual void dispatch ( geom const* pNode );
    virtual void dispatch ( geomFx const* pNode );
    virtual void dispatch ( group const* pNode );
    virtual void dispatch ( light const* pNode );
    virtual void dispatch ( sndEffect const* pNode );
    virtual void dispatch ( sndListener const* pNode );
    
    virtual void dispatch ( blend const* pState );
    virtual void dispatch ( cull const* pState );
    virtual void dispatch ( depth const* pState );
    virtual void dispatch ( lighting const* pState );
    virtual void dispatch ( lightPath const* pState );
    virtual void dispatch ( material const* pState );
//     virtual void dispatch ( polygonMode const* pState );
    virtual void dispatch ( texEnv const* pState );
//     virtual void dispatch ( texGen const* pState );
    virtual void dispatch ( texture const* pState );
    virtual void dispatch ( textureXform const* pState );
    
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneddogllist_h


