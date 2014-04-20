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
#include "imgcoreimage.h"
#include "imgpvr.h"
#include "pnidbg.h"

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

namespace img {

factory::factory ()
{
  addLoader("dds", [] ( std::string const& fname ) { return dds::loadHelper ( fname ); } );
//  addLoader("tga", [] ( std::string const& fname ) { return targa::loadHelper ( fname ); } );

#if defined __IPHONE_7_0 || defined __MAC_10_9

  auto coreImageFunc = [] ( std::string const& fname ) { return coreImage::loadHelper(fname); };

  addLoader("jpg", coreImageFunc);
  addLoader("jpeg", coreImageFunc);
  addLoader("png", coreImageFunc);
  addLoader("tif", coreImageFunc);
  addLoader("tiff", coreImageFunc);
  addLoader("gif", coreImageFunc);

  auto pvrImageFunc = [] ( std::string const& fname ) { return pvr::loadHelper(fname); };

  addLoader("pvr",pvrImageFunc);
  addLoader("pvrtc",pvrImageFunc);

#endif

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

base* factory::loadSync ( std::string const& cfname )
{
  std::string fname;

  if ( mSearchPath.resolve(cfname, fname))
  {
    std::string extension = toLower ( pni::pstd::searchPath::ext(fname) );

    auto found = mLoadFunctions.find ( extension );
    if ( found != mLoadFunctions.end () )
      return found->second ( fname );
    else
      PNIDBGSTR("could not find loader for " << cfname);
  }
  else
    PNIDBGSTR("could not resolve file for " << cfname);

  return nullptr;
}


void factory::cancel ( LoadFuture const& loadFuture )
{

}

void factory::addLoader ( std::string const& extension, LoadFunction func )
{
  mLoadFunctions[ toLower ( extension ) ] = func;
}

void factory::remLoader ( std::string const& extension )
{
  auto found = mLoadFunctions.find ( toLower ( extension ) );
  if ( found != mLoadFunctions.end () )
    mLoadFunctions.erase ( found );
}

} // end namespace img

