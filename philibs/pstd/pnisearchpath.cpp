/////////////////////////////////////////////////////////////////////
//
//  pnisearchpath.cpp
//
/////////////////////////////////////////////////////////////////////

#include "pnisearchpath.h"

/////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

/////////////////////////////////////////////////////////////////////

namespace pni {
  namespace pstd {


bool searchPath::doStat ( std::string const& fname )
{
  bool retVal = false;

  if ( FILE* fid = fopen ( fname.c_str (), "rb" ) )
  {
    struct stat fileStat;
    memset ( &fileStat, 0, sizeof ( fileStat ) );

    if ( fstat ( fileno ( fid ), &fileStat ) == 0 )
      retVal = true;

    fclose ( fid );
  }

  return retVal;
}

std::string searchPath::tail ( std::string const& src )
{
  std::string tmp = src;
  size_t ind = src.find_last_of ( "/\\" );
  if ( ind != std::string::npos )
    tmp = src.substr ( ind + 1 );

  return tmp;
}

std::string searchPath::head ( std::string const& src )
{
  std::string tmp = src;
  size_t ind = src.find_last_of ( "/\\" );
  if ( ind != std::string::npos )
    tmp = src.substr ( 0, ind );

  return tmp;
}

std::string searchPath::root ( std::string const& src )
{
  std::string tmp = src;
  size_t ind = src.find_last_of ( ".\\" );
  if ( ind != std::string::npos )
    tmp = src.substr ( 0, ind );

  return tmp;
}

std::string searchPath::ext ( std::string const& src )
{
  std::string tmp = src;
  size_t ind = src.find_last_of ( ".\\" );
  if ( ind != std::string::npos )
    tmp = src.substr ( ind + 1 );

  return tmp;
}

bool searchPath::resolve ( std::string const& fname, std::string& ret )
{
  std::string fnameTail = tail ( fname );

  for ( auto& path : mPaths )
  {
    std::string testLong = path + "/" + fname;
    std::string testShort = path + "/" + fnameTail;

    if ( doStat ( testShort ) )
    {
      ret = testShort;
      return true;
    }

    if ( doStat ( testLong ) )
    {
      ret = testLong;
      return true;
    }
  }

  return false;
}

searchPath& searchPath::operator += ( searchPath const& rhs )
{
  for ( auto& path : rhs.mPaths )
    mPaths.push_back(path);
  return *this;
}

void searchPath::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{

}

/////////////////////////////////////////////////////////////////////

  } // end namespace pstd
} // end namespace pni
