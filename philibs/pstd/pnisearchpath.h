/////////////////////////////////////////////////////////////////////
//
//	class: searchPath
//
/////////////////////////////////////////////////////////////////////

#ifndef pnisearchpath_h
#define pnisearchpath_h

/////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <cstdio>

#include "pnirefcount.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
  namespace pstd {

/////////////////////////////////////////////////////////////////////

class searchPath :
  public refCount
{
  public:

    void addPath ( std::string const& path ) { mPaths.push_back ( path ); }
    void remPath ( std::string const& path )
      {
        auto found = std::find ( mPaths.begin (), mPaths.end (), path );
        if ( found != mPaths.end () )
          mPaths.erase ( found );
      }

    void clearPaths () { mPaths.clear(); }

      /// Find fname in the added paths.
      /// @param fname The file name to find.  fname will be added to
      /// each search path, and so will the tail of fname, until a
      /// match is found.
      /// @param ret the resolved file name, if found (return is true)
      /// @return true if file is found, ret will be valid.
    bool resolve ( std::string const& fname, std::string& ret );

      // Helper methods
      /// @return true if file 'fname' exists
    static bool doStat ( std::string const& fname );

      /// @return tail of fname ( e.g., "bar/foo.gif" => "foo.gif" )
    static std::string tail ( std::string const& src );

      /// @return head of fname ( e.g., "bar/foo.gif" => "bar" )
    static std::string head ( std::string const& src );

      /// @return head of fname ( e.g., "bar/foo.gif" => "bar/foo" )
    static std::string root ( std::string const& src );

      /// @return extension of fname ( e.g., "bar/foo.gif" => "gif" )
    static std::string ext ( std::string const& src );

  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;

  private:
    typedef std::vector< std::string > Paths;
    Paths mPaths;

};

/////////////////////////////////////////////////////////////////////

  } // end namespace pstd
} // end namespace pni

#endif // pnisearchpath_h
