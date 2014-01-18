/////////////////////////////////////////////////////////////////////
//
//    class: factory
//
/////////////////////////////////////////////////////////////////////

#ifndef loaderloaderfactory_h
#define loaderloaderfactory_h

/////////////////////////////////////////////////////////////////////

#include "sceneloaderbase.h"
#include "pnithreadpool.h"
#include "pniautoref.h"

#include <string>
#include <functional>
#include <unordered_map>

class scene::node;

/////////////////////////////////////////////////////////////////////

namespace loader {

class factory
{
  public:
    factory ();

      /// Singleton accessor.
    static factory& getInstance ();

    typedef std::future< scene::node* > LoadFuture;

      /// Load an image file asynchronously and receive callback when done.
    LoadFuture loadAsync ( std::string const& fname );
      /// Load an image synchronously.
    scene::node* loadSync ( std::string const& fname );
      /// Cancel a pending image load.
      /// @note Not implemented yet!!!
    void cancel ( LoadFuture const& loadFuture );

      /// Register loaderBase instances with the extension they handle.
    void addLoader ( std::string const& extension, loader::base* pLoader );
    void remLoader ( std::string const& extension );

  private:
    typedef pni::pstd::autoRef<loader::base> BaseRef;
    typedef std::unordered_map< std::string, BaseRef > Loaders;

      // TODO: Implement a work queue thread to handle file reads.
    pni::pstd::threadPool mThreadPool { 2 };
    Loaders mLoaders;


};


} // end namespace loader

#endif // loaderloaderfactory_h
