/////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

#include "sceneloaderase.h"

#include "sceneisectdd.h"

#include "timerprof.h"

/////////////////////////////////////////////////////////////////////

using namespace loader;
using namespace scene;
  
/////////////////////////////////////////////////////////////////////

void doIsect ( node* pNode )
{
  typedef pni::pstd::autoRef< isectTest > TestRef;
  TestRef aTest = new isectTest;
  
  aTest->mSegs.resize ( 1 );
  aTest->mSegs[ 0 ].set ( 
      pni::math::vec3 ( 20.0f, 0.0f, 250.0f ), 
      pni::math::vec3 ( 20.0f, 0.0f, -250.0f ) );
  
  aTest->initHits ();
  
  isectDd* mIsect = new isectDd;
  
  mIsect->setTest ( aTest.get () );
  mIsect->startGraph ( pNode );
  
  float tval = aTest->mHits[ 0 ].mTval;
std::cout << "tval = " << tval << std::endl;
  
  // TODO: Do something with tval.
}

int main ( int argc, char* argv[] )
{
  if ( argc != 2 )
    return -1;

  ase* pAse = new ase;

  node* pNode = pAse->load ( argv[ 1 ]);
      
  doIsect ( pNode );

  return 0;
}


