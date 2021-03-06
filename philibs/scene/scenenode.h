/////////////////////////////////////////////////////////////////////
//
//    class: node
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenenode_h
#define scenescenenode_h

/////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include "scenetypes.h"
#include "scenestate.h"
#include "pnirefcount.h"
#include "pniautoref.h"
#include "sceneattr.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  class graphDd;
  class stateDd;
  
/////////////////////////////////////////////////////////////////////

class node :
    public pni::pstd::refCount,
    public travDataContainer,
    public travDataMaskContainer<>
{
  public:
    node() = default;
    virtual ~node();
    node(node const& rhs) = default;
//      node& operator=(node const& rhs);
//      bool operator==(node const& rhs) const;
    
    typedef pni::pstd::autoRef< node > NodeRef;
    typedef std::vector< NodeRef > Nodes;
    typedef std::vector< node* > Parents;
    typedef pni::pstd::autoRef< state > StateRef;
    typedef std::unordered_multimap< state::Id, StateRef, std::hash< int > > States;

    // Node framework methods.
    virtual node* dup () const = 0;
    virtual void accept ( graphDd* pDd ) const = 0;

    // Node concrete methods.
    void setName ( std::string const& str ) { mName = str; }
    std::string const& getName () const { return mName; }
    
    matrix4& matrixOp () { 
        mMatrix.setIsIdent ( false ); setBoundsDirty (); return mMatrix; }
    matrix4 & getMatrix () { return mMatrix; }
    matrix4 const& getMatrix () const { return mMatrix; }

    virtual void setBoundsDirty ();
    box3 const& getBounds () const;
    void getCenterInParent ( pni::math::vec3& center ) const;
    
    // The non-const methods automatically call setBoundsDirty.
    // Maybe that's overzealous for getChildren.
    void addChild ( node* pNode );
    void remChild ( node* pNode );
    void remAllChildren  ();
    Nodes& childrenOp () { setBoundsDirty (); return mChildren; }
    Nodes& getChildren () { return mChildren; } // For non-const, ro iteration.
    Nodes const& getChildren () const { return mChildren; }
   
    // TODO Need non-const parent interface too?
    void remParent ( node* pNode );
    void remAllParents ();  // Refs can go to zero!!!
    Parents& getParents () { return mParents; }
    Parents const& getParents () const { return mParents; }
    
    void setState ( state* pState, state::Id id );
    void setState ( state* pState, state::Id id, TravMaskType mask );
    void remState ( state* pState );
    void remState ( state::Id id );
    void remState ( state::Id id, MaskType mask );
    state* getState ( state::Id id ) const;
    state* getState ( state::Id id, MaskType mask ) const;
    States& getStates () { return mStates; }
    States const& getStates () const { return mStates; }
    
    void uniquifyStates ();
    void uniquifyState ( state::Id id );
    
    // TODO? attrs
    
  protected:
    // Node framework methods.
    void setParentBoundsDirty ();
    virtual void updateBounds () const;
    
    matrix4 mMatrix;
    mutable box3 mBounds;

  private:
    Nodes mChildren;
    Parents mParents;
    States mStates;
    std::string mName;

    // From refCount
  protected:
    virtual void onDoDbg ();

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenenode_h


