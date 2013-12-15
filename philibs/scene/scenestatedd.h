/////////////////////////////////////////////////////////////////////
//
//    class: stateDd
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenestatedd_h
#define scenescenestatedd_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include "scenestate.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  class node;

  class blend;
  class cull;
  class depth;
  class lighting;
  class lightPath;
  class material;
//   class polygonMode;
  class texEnv;
//   class texGen;
  class texture;
  class textureXform;
  
/////////////////////////////////////////////////////////////////////

class stateDd 
{
  public:
    friend class blend;
    friend class cull;
    friend class depth;
    friend class lighting;
    friend class lightPath;
    friend class material;
//     friend class polygonMode;
    friend class texEnv;
//     friend class texGen;
    friend class texture;
    friend class textureXform;

    stateDd();
    virtual ~stateDd();
//     stateDd(stateDd const& rhs);
//     stateDd& operator=(stateDd const& rhs);
//     bool operator==(stateDd const& rhs) const;

    virtual void startStates ( node const* pNode ) = 0;
    virtual void setDefaultState ( state const* pState, state::Id id ) = 0;
    
  protected:
  
    virtual void dispatch ( blend const* pState ) = 0;
    virtual void dispatch ( cull const* pState ) = 0;
    virtual void dispatch ( depth const* pState ) = 0;
    virtual void dispatch ( lighting const* pState ) = 0;
    virtual void dispatch ( lightPath const* pState ) = 0;
    virtual void dispatch ( material const* pState ) = 0;
//     virtual void dispatch ( polygonMode const* pState ) = 0;
    virtual void dispatch ( texEnv const* pState ) = 0;
//     virtual void dispatch ( texGen const* pState ) = 0;
    virtual void dispatch ( texture const* pState ) = 0;
    virtual void dispatch ( textureXform const* pState ) = 0;

    
    
  private:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene

#endif // scenescenestatedd_h


