/////////////////////////////////////////////////////////////////////
//
//    class: factory
//
/////////////////////////////////////////////////////////////////////

#ifndef imgimgfactory_h
#define imgimgfactory_h

/////////////////////////////////////////////////////////////////////

#include "imgbase.h"
#include "pnithreadpool.h"
#include "pnisearchpath.h"

#include <string>
#include <functional>
#include <unordered_map>

/////////////////////////////////////////////////////////////////////

namespace img {

class factory
{
  public:
    factory ();

      /// Singleton accessor.
    static factory& getInstance ();

      // Load fname, with callback.
    typedef std::future< base* > LoadFuture;
    typedef std::function< base* ( std::string const& fname ) > LoadFunction;

      /// Load an image file asynchronously and receive callback when done.
    LoadFuture loadAsync ( std::string const& fname );
      /// Load an image synchronously.
    base* loadSync ( std::string const& fname );
      /// Cancel a pending image load.
      /// @note Not implemented yet!!!
    void cancel ( LoadFuture const& loadFuture );

      /// Register loaderBase instances with the extension they handle.
    void addLoader ( std::string const& extension, LoadFunction func );
    void remLoader ( std::string const& extension );

      /// Manage associated search path
    pni::pstd::searchPath mSearchPath;

  private:
    typedef std::unordered_map< std::string, LoadFunction > LoadFunctions;

      // TODO: Implement a work queue thread to handle file reads.
    pni::pstd::threadPool mThreadPool { 2 };
    LoadFunctions mLoadFunctions;

};


} // end namespace img

#endif // imgimgfactory_h
