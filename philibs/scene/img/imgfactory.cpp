/////////////////////////////////////////////////////////////////////
//
//    class: factory
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

#include "imgfactory.h"
#include "pnisearchpath.h"
#include "imgdds.h"
#include "imgtarga.h"

#include <string>

/////////////////////////////////////////////////////////////////////

namespace img {

factory::factory ()
{
  addLoader("dds", [] ( std::string const& fname ) { return dds::loadHelper ( fname ); } );
//  addLoader("tga", [] ( std::string const& fname ) { return targa::loadHelper ( fname ); } );
}

factory::LoadFuture factory::loadAsync ( std::string const& fname )
{
  return mThreadPool.enqueue( [ this ]( std::string const& fname) { return this->loadSync( fname ); }, fname );
}

base* factory::loadSync ( const std::string &fname )
{
  std::string extension = pni::pstd::searchPath::ext(fname);

  auto found = mLoadFunctions.find ( extension );
  if ( found != mLoadFunctions.end () )
    return found->second ( fname );
  else
    return nullptr; // Temp
}


void factory::cancel ( LoadFuture const& loadFuture )
{

}

void factory::addLoader ( std::string const& extension, LoadFunction func )
{
  mLoadFunctions[ extension ] = func;
}

void factory::remLoader ( std::string const& extension )
{
  auto found = mLoadFunctions.find ( extension );
  if ( found != mLoadFunctions.end () )
    mLoadFunctions.erase ( found );
}

} // end namespace img

