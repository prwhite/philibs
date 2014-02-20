/////////////////////////////////////////////////////////////////////
//
//    class: factory
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

#include "sceneloaderfactory.h"
#include "sceneloaderase.h"
#include "sceneloaderassimp.h"
#include "pnisearchpath.h"

#include <string>
#include <algorithm>

/////////////////////////////////////////////////////////////////////

namespace {
  std::string toLower ( std::string const& src )
    {
      std::string ret = src;
      std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
      return ret;
    }
}

namespace loader {

factory::factory ()
{
//  addLoader("dds", [] ( std::string const& fname ) { return dds::loadHelper ( fname ); } );

  addLoader ( "ase", new loader::ase );

  loader::assimp* pAssimp = new loader::assimp;

  addLoader ( "stl", pAssimp );
  addLoader ( "obj", pAssimp );
  addLoader ( "stl", pAssimp );
  addLoader ( "dae", pAssimp );
}

factory& factory::getInstance ()
{
  static factory* pFactory = 0;
  if ( ! pFactory )
    pFactory = new factory;
  return *pFactory;
}

factory::LoadFuture factory::loadAsync ( std::string const& fname )
{
  return mThreadPool.enqueue( [ this ]( std::string const& fname) { return this->loadSync( fname ); }, fname );
}

scene::node* factory::loadSync (  std::string const& cfname )
{
  std::string fname;
  if ( mSearchPath.resolve(cfname, fname))
  {
    std::string extension = toLower ( pni::pstd::searchPath::ext(fname) );

    auto found = mLoaders.find ( extension );
    if ( found != mLoaders.end () )
      return found->second->load ( fname );
  }

  return nullptr;
}


void factory::cancel ( LoadFuture const& loadFuture )
{

}

void factory::addLoader ( std::string const& extension, loader::base* pBase )
{
  mLoaders[ toLower ( extension ) ] = pBase;
}

void factory::remLoader ( std::string const& extension )
{
  auto found = mLoaders.find ( toLower ( extension ) );
  if ( found != mLoaders.end () )
    mLoaders.erase ( found );
}

} // end namespace loader

