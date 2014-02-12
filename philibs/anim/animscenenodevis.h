/////////////////////////////////////////////////////////////////////
//
//  class: sceneNodeVis

// This is a bit of an experiment.  We are using these animations
// as one-shots.  I.e., they should not have a duration in practice,
// and we are leveraging the capability of the anim::manager to always
// give finishing animations a call with tval == 1.0f so they can
// finish in the right spot.  In this case, when the internalUpdate
// is called, it will flip visibility state.

//
/////////////////////////////////////////////////////////////////////

#ifndef animanimscenenodevis_h
#define animanimscenenodevis_h

/////////////////////////////////////////////////////////////////////

#include "animbase.h"

#include "pniautoref.h"

#include "scenenode.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

class sceneNodeVis :
  public base
{
  public:
    sceneNodeVis ();
    //virtual ~sceneNodeVis ();
    //sceneNodeVis ( sceneNodeVis const& rhs );
    //sceneNodeVis& operator= ( sceneNodeVis const& rhs );
    //bool operator== ( sceneNodeVis const& rhs ) const;

    typedef pni::pstd::autoRef< scene::node > NodeRef;
    
    void setNode ( scene::node* pNode ) { mNode = pNode; }
    scene::node* getNode () const { return mNode.get (); }

      // Defaults to 0x00 -> invisible.
    void setMask ( scene::node::MaskType mask ) { mMask = mask; }
    scene::node::MaskType getMask () const { return mMask; }

  protected:
    NodeRef mNode;
    scene::node::MaskType mMask;

  private:


    // interface from anim::base
  public:
    virtual void internalUpdate ( manager* pManager, TimeType tval );

  protected:

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimscenenodevis_h


