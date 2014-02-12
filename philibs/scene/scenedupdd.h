/////////////////////////////////////////////////////////////////////
//
//    class: dupDd
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenedupdd_h
#define scenescenedupdd_h

/////////////////////////////////////////////////////////////////////

#include "scenegraphdd.h"
#include "scenestatedd.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class dupDd :
  public graphDd,
  public stateDd
{
  public:
    dupDd ();
    //virtual ~dupDd();
    //dupDd(dupDd const& rhs);
    //dupDd& operator=(dupDd const& rhs);
    //bool operator==(dupDd const& rhs) const;
    
    enum Opts
    {
      None = 0x00,
      UniquifyGeomData = 0x01,
      UniquifyStates = 0x02,
      All = UniquifyGeomData | UniquifyStates
    };
    
      // Default opts is == None.
    void setDupOpts ( unsigned int opts ) { mOpts = opts; }
    unsigned int getDupOpts () const { return mOpts; }
    
  protected:
      
  private:
    //typedef pni::pstd::autoRef< node > NodeRef;
    typedef node* NodeRef;
    typedef std::vector< NodeRef > NodeStack;
    
    NodeRef mRetNode;
    NodeStack mNodeStack;
    unsigned int mOpts;
    state::Id mCurId;

    // interface from graphDd
  public:

      // graphDd framework:
      // TRICKY: We don't use this... it's a no-op.  Use dupGraph method
      // instead to get result of dup traversal.
    virtual void startGraph ( node const* pNode );
    
    node* dupGraph ( node const* pNode );
    
  protected:
  
    virtual void dispatch ( camera const* pNode );
    virtual void dispatch ( geom const* pNode );
    virtual void dispatch ( group const* pNode );
    virtual void dispatch ( light const* pNode );
  	virtual void dispatch ( geomFx const* pNode );
    virtual void dispatch ( sndEffect const* pNode );
    virtual void dispatch ( sndListener const* pNode );
  	
    void dispatchChildren ( node const* pNode );
    void dispatchNode ( node const* pNode );
    void dispatchNodePush ( node const* pNode );
    void dispatchNodePre ( node const* pNode );
    void dispatchNodePost ( node const* pNode );

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const
      {
        refs.push_back(mRetNode);
      }
  
    // interface from stateDd
  public:
    virtual void startStates ( node const* pNode );
    virtual void setDefaultState ( state const* pState, state::Id id );
    
  protected:

    void dispatchState ( state const* pState );
  
    virtual void dispatch ( blend const* pState );
    virtual void dispatch ( cull const* pState );
    virtual void dispatch ( depth const* pState );
    virtual void dispatch ( lighting const* pState );
    virtual void dispatch ( lightPath const* pState );
    virtual void dispatch ( material const* pState );
    virtual void dispatch ( prog const* pState );
    virtual void dispatch ( texEnv const* pState );
    virtual void dispatch ( texture const* pState );
    virtual void dispatch ( textureXform const* pState );
    virtual void dispatch ( uniform const* pState );


};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenedupdd_h


