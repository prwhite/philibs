/////////////////////////////////////////////////////////////////////
//
//  file: fontfontfactory.cpp
//
/////////////////////////////////////////////////////////////////////

#include "fontfontfactory.h"

#include "pnirefcountdbg.h"

/////////////////////////////////////////////////////////////////////

namespace font {
    
/////////////////////////////////////////////////////////////////////

//fontFactory::fontFactory()
//{
//  
//}
//
//fontFactory::~fontFactory()
//{
//  
//}
//
//fontFactory::fontFactory(fontFactory const& rhs)
//{
//  
//}
//
//fontFactory& fontFactory::operator=(fontFactory const& rhs)
//{
//  return *this;
//}
//
//bool fontFactory::operator==(fontFactory const& rhs) const
//{
//  return false;
//}

fontFactory& fontFactory::getInstance ()
{
  static fontFactory* pFactory = 0;
  if ( ! pFactory )
    pFactory = new fontFactory;
  
  return *pFactory;
}

font* fontFactory::load ( std::string const& fname, std::string const& textureFname )
{
  std::lock_guard<std::mutex> guard ( mFontMapMutex );

  auto found = mFontMap.find ( fname );
  
  if ( found != mFontMap.end () )
  {
    return found->second.get();
  }
  else
  {
    std::string realFname;
    std::string realTextureFname;
    
    if( mSearchPath.resolve(fname, realFname) && mSearchPath.resolve(textureFname, realTextureFname) )
    {
      font* pFont = new font;
    
      if ( pFont->load(realFname, realTextureFname) )
      {
        mFontMap[ fname ] = pFont;
        return pFont;
      }
      else
      {
        FontRef ref ( pFont );  // Will cause delete of bad font.
        return nullptr;
      }
    }
    else
      return nullptr;
  }
}

fontFactory::FontFuture fontFactory::loadAsync ( std::string const& fname, std::string const& textureFname )
{
  return mThreadPool.enqueue([&]() { return load(fname, textureFname); } );
}
  
void fontFactory::unload ( std::string const& fname )
{
  std::lock_guard<std::mutex> guard ( mFontMapMutex );
  mFontMap.erase(fname);
}

/////////////////////////////////////////////////////////////////////
// overrides from pni::pstd::refCount

void fontFactory::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  pni::pstd::dbgRefCount::collectMapSecondRefs(mFontMap, refs);
}

/////////////////////////////////////////////////////////////////////


} // end of namespace font 


