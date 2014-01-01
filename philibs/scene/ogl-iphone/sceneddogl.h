/////////////////////////////////////////////////////////////////////
//
//    class: ddOgl
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenegraphddogl_h
#define scenescenegraphddogl_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include "scenegraphdd.h"
#include "scenestatedd.h"
#include "pnifrustum.h"
#include "scenenodepath.h"

#include "sceneddogllist.h"


/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class ddOgl :
  public graphDd,
  public stateDd
{
  public:
    ddOgl();
    virtual ~ddOgl();
//     ddOgl(ddOgl const& rhs);
//     ddOgl& operator=(ddOgl const& rhs);
//     bool operator==(ddOgl const& rhs) const;

    // From graphDd.
    virtual void startGraph ( node const* pNode );
    virtual void bindTextures ( node const* pNode ) 
        { mDdList.bindTextures ( pNode ); }
    
    // From stateDd.
    virtual void setDefaultState ( state const* pState, state::Id id );
    virtual void startStates ( node const* pNode );

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
    virtual void dispatch ( prog const* pState );
    virtual void dispatch ( texEnv const* pState );
//     virtual void dispatch ( texGen const* pState );
    virtual void dispatch ( texture const* pState );
    virtual void dispatch ( textureXform const* pState );
    virtual void dispatch ( uniform const* pState );
  
  private:
  
    // Bool return indicates whether node was culled or not.
    void setupCamera ( node const* pNode );
    float calcDistSqr ( node const* pNode );
    bool dispatchPre ( node const* pNode );
    bool testInViewFrustum ( node const* pNode );
    void applyStates ( node const* pNode );
    void dispatchChildren ( node const* pNode );
    void dispatchPost ( node const* pNode );
    
    typedef ddOglList::stateSet stateSet;
    typedef ddOglList::StdStateStack StdStateStack;
    typedef ddOglList::MatStack MatStack;
    typedef ddOglList::MatStack MatInvStack;
    typedef ddOglList::BoundsStack BoundsStack;
    
    // Data members.
    nodePath mNodePath;
    StdStateStack mStateStack;
    MatStack mMatStack;
    MatStack mMatInvStack;
    BoundsStack mBoundsStack;
    
    
    pni::math::matrix4 mCamMat;       // Not currently used.
    pni::math::vec3 mCamPos;          // In world frame.
    pni::math::frustum mCamFrustum;   // Not currently used.
    
    ddOglList mDdList;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenegraphddogl_h


