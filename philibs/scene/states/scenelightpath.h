/////////////////////////////////////////////////////////////////////
//
//    class: lightPath
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenelightpath_h
#define scenescenelightpath_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"
#include "scenenodepath.h"
#include "scenelight.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

class lightPath :
  public state
{
  public:
    lightPath();
//     virtual ~lightPath();
//     lightPath(lightPath const& rhs);
//     lightPath& operator=(lightPath const& rhs);
//     bool operator==(lightPath const& rhs) const;
    
    enum Constants
    {
       MaxNodePaths = 8 // HACK Magic Number for GL lighting.
    };
    
    typedef std::vector< nodePathNoRef > NodePaths;
    
//     NodePaths& getNodePaths () { return mNodePaths; }
    NodePaths const& getNodePaths () const { return mNodePaths; }
    
//     // First func is for non-instanced lights.
//     void addLight ( node* pLight ) { mNodePaths.push_back ( nodePath ( pLight ) ); }
//     void addLightPath ( nodePath const& path ) { mNodePaths.push_back ( path ); }

    void setLightPath ( nodePath const& path, size_t which ) 
        { 
          mNodePaths[ which ] = path;
        }

    void setLight ( light* pLight, size_t which ) 
        { 
          //mNodePaths[ which ] = nodePath ( pLight );
          mNodePaths[ which ].init ( pLight );
        }

    nodePath & getNodePath ( size_t which )
        { 
          return mNodePaths[ which ];
        }

    nodePath const& getNodePath ( size_t which ) const
        { 
          return mNodePaths[ which ];
        }


  protected:
    
  private:
    
    NodePaths mNodePaths;

  // interface from state
  public:
    virtual state* dup () const { return new lightPath ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenelightpath_h


