/////////////////////////////////////////////////////////////////////
//
//  class: fontFactory
//
/////////////////////////////////////////////////////////////////////

#ifndef fontfontfontfactory_h
#define fontfontfontfactory_h

/////////////////////////////////////////////////////////////////////

#include "fontfont.h"
#include "pnirefcount.h"
#include "pniautoref.h"
#include "pnithreadpool.h"
#include "pnisearchpath.h"

#include <map>
#include <functional>
#include <mutex>

/////////////////////////////////////////////////////////////////////

namespace font {
    
// ///////////////////////////////////////////////////////////////////

class fontFactory :
    public pni::pstd::refCount
{
  public:
//    fontFactory();
//    virtual ~fontFactory();
//    fontFactory(fontFactory const& rhs);
//    fontFactory& operator=(fontFactory const& rhs);
//    bool operator==(fontFactory const& rhs) const;

    static fontFactory& getInstance ();
  
      /// The fontFactory not only loads fonts, it's also a cache for fonts.
      /// The load method will put successfully loaded fonts in its cache.
    font* load ( std::string const& fname, std::string const& textureFname );
  
    using FontFuture = std::future< font* >;
  
  
    FontFuture loadAsync ( std::string const& fname, std::string const& textureFname );
  
      /// The unload method will purge the named font from its cache.
      /// @note Other objects might still be referencing the font.  It is
      /// generally the application's responsibility to track these kinds of
      /// things.
    void unload ( std::string const& fname );

    pni::pstd::searchPath mSearchPath;

  protected:
    
  private:
    using FontRef = pni::pstd::autoRef< font >;
    using FontMap = std::map< std::string, FontRef >;
  
    FontMap mFontMap;
    pni::pstd::threadPool mThreadPool { 1 };
    std::mutex mFontMapMutex;
    
    // interface from pni::pstd::refCount
  public:
  
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace font 

#endif // fontfontfontfactory_h


