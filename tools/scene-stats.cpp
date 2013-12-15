/////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

#include "sceneloaderase.h"

#include "scenegraphdd.h"

#include "scenegroup.h"
#include "scenegeom.h"
#include "scenelight.h"
#include "scenecamera.h"

#include <iostream>

/////////////////////////////////////////////////////////////////////

using namespace scene;
using namespace std;

class ddStats :
  public graphDd
{
    unsigned int mGeoms;
    unsigned int mLights;
    unsigned int mGroups;
    unsigned int mCameras;
    unsigned int mTris;

  public:
    ddStats () :
      mGeoms ( 0 ),
      mLights ( 0 ),
      mGroups ( 0 ),
      mCameras ( 0 ),
      mTris ( 0 )
        {
          
        }
    
    
    // graphDd framework:
    virtual void startGraph ( node const* pNode )
        {
          pNode->accept ( this );
          
          cout << "num geoms = " << mGeoms  << endl;
          cout << "num lights = " << mLights  << endl;
          cout << "num groups = " << mGroups  << endl;
          cout << "num cameras = " << mCameras  << endl;
          cout << "num tris = " << mTris  << endl;
        }
    
  protected:
    
    virtual void dispatch ( camera const* pNode )
        {
          mCameras++;
        }
        
    virtual void dispatch ( geom const* pNode )
        {
          mGeoms++;
          
          geomData* pData = pNode->mGeomData.get ();
          mTris += pData->getIndices ().size () / 3;
        }
        
    virtual void dispatch ( group const* pNode )
        {
          mGroups++;
        
          typedef node::Nodes::const_iterator ConstIter;
          node::Nodes const& kids = pNode->getChildren ();

          ConstIter end = kids.end ();          
          for ( ConstIter cur = kids.begin (); cur != end; ++cur )
            ( *cur )->accept ( this );
        }
        
    virtual void dispatch ( light const* pNode )
        {
          mLights++;
        }
    
};


/////////////////////////////////////////////////////////////////////

int main ( int argc, char* argv[] )
{
  if ( argc != 2 )
    return -1;

  using namespace loader;
  using namespace scene;
  
  ase* pAse = new ase;
  
  if ( node* pNode = pAse->load ( argv[ 1 ]) )
  {
    ddStats stats;
    stats.startGraph ( pNode );
  }

  return 0;
}


