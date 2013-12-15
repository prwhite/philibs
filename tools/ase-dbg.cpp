/////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

#include "sceneloaderase.h"

#include "timerprof.h"

/////////////////////////////////////////////////////////////////////

int main ( int argc, char* argv[] )
{
  if ( argc != 2 )
    return -1;

  using namespace loader;
  using namespace scene;
  
  ase* pAse = new ase;

	pstd::timerProf tprof ( "load time", 1 );
	tprof.start ();
  
  node* pNode = pAse->load ( argv[ 1 ]);
  
  loader::directory* pDir = pAse->getDirectory ();
  
  if ( pDir->getNode ( "dome" ) )
    std::cout << "found node \"dome\"" << std::endl;
    
  loader::directory::NodeRange pair = pDir->getNodes ( "core" );
  
  for ( ; pair.first != pair.second; ++pair.first )
    std::cout << "found node from iters: \"core\"" << std::endl;

	tprof.stop ();

  return 0;
}


