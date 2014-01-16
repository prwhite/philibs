/////////////////////////////////////////////////////////////////////
//
//    class: dbgDd
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenedbgdd_h
#define scenescenedbgdd_h

/////////////////////////////////////////////////////////////////////

#include "scenegraphdd.h"
#include "scenestatedd.h"

#include <set>
#include <string>
#include <iostream>

/////////////////////////////////////////////////////////////////////

namespace scene {

  class texture;
  class geomData;
    
/////////////////////////////////////////////////////////////////////

class dbgDd :
  public graphDd,
  public stateDd
{
  public:
    dbgDd( std::ostream& str = std::cout );
    //virtual ~dbgDd();
    //dbgDd(dbgDd const& rhs);
    //dbgDd& operator=(dbgDd const& rhs);
    //bool operator==(dbgDd const& rhs) const;
    
    enum Dbg
    {
      Nodes = 0x01,
      NodesFull = 0x02,
      States = 0x04,
      StatesFull = 0x08,
      TravData = 0x0f,
      TravDataFull = 0x10, 
      Xform = 0x20,
      Bounds = 0x40,
      TextureMemTotal = 0x80,
      GeomDataMemTotal = 0xf0,
      All = Nodes | NodesFull | 
          States | StatesFull | 
          TravData | TravDataFull |
          Xform | Bounds |
          TextureMemTotal | GeomDataMemTotal
    };
    
    void setDbgOpts ( unsigned int opts ) { mOpts = opts; }
    unsigned int getDbgOpts () const { return mOpts; }
    
  protected:
      
    typedef std::set< scene::texture const* > TexSet;
    typedef std::set< scene::geomData const* > GeomDataSet;
      
  private:
    size_t mIndent;
    unsigned int mOpts;
    TexSet mTexSet;
    GeomDataSet mGeomDataSet;
    std::ostream& mStr;

  // interface from graphDd
  public:

    // graphDd framework:
    virtual void startGraph ( node const* pNode );
    
  protected:
    
    virtual void dispatch ( camera const* pNode );
    virtual void dispatchGeom ( geom const* pNode );
    virtual void dispatch ( geom const* pNode );
    virtual void dispatch ( geomFx const* pNode );
    virtual void dispatch ( group const* pNode );
    virtual void dispatch ( light const* pNode );
    virtual void dispatch ( sndEffect const* pNode );
    virtual void dispatch ( sndListener const* pNode );
  	
    void dispatchChildren ( node const* pNode );
    void dispatchNode ( node const* pNode );
    void dispatchTravData ( node const* pNode );
    void dispatchTextureMem ( node const* pNode );
    void dispatchXform ( node const* pNode );
    void dispatchBounds ( node const* pNode );
    void doTotals ();
    std::ostream& indent ( std::string const& str = "  " );
    
    // interface from stateDd
  public:
    virtual void startStates ( node const* pNode );
    virtual void setDefaultState ( state const* pState, state::Id id );
    
  protected:
  
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


};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenedbgdd_h


