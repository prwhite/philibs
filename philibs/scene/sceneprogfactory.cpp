/////////////////////////////////////////////////////////////////////
//
//  class implementation: progFactory
//
/////////////////////////////////////////////////////////////////////

#include "sceneprogfactory.h"
#include "pnidbg.h"

#include <fstream>

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

progFactory& progFactory::getInstance ()
{
  static progFactory* pFactory = 0;
  if ( ! pFactory )
    pFactory = new progFactory;
  return *pFactory;
}

prog* progFactory::genProg ( progProps const& props )
{
    // TODO: return something real
  return nullptr;
}

void progFactory::purgeCache ()
{
    // TODO: do iteration, while doing some erasing, efficiently
}

/////////////////////////////////////////////////////////////////////

namespace {

  // This should be in common code somewhere.
bool readFile ( std::string const& fname, std::string& dst )
{
  std::ifstream istr ( fname );
  
  if ( istr.good())
  {
    istr.seekg(0, istr.end);
    std::istream::pos_type len = istr.tellg();
    istr.seekg(0, istr.beg);
    
    dst.resize( (size_t) len);
    
    istr.read(&dst[ 0 ], (size_t) len);
    return true;
  }
  else
    return false;
}

  // This is idiotic. PRW owns it.
template < class Type >
struct cleanupOrNot
{
  Type* mPtr;
  
  operator Type* () { return mPtr; }
  Type* operator -> () { return mPtr; }
  Type* good () { Type* tmp = mPtr; mPtr = 0; return tmp; }
  
  ~cleanupOrNot () { if ( mPtr ) delete mPtr; }
};

} // end namespace anon

prog* progFactory::loadSync ( Filenames const& fnames )
{
  cleanupOrNot<prog> pProg { new prog };   // Let's be careful not to leak this if we have errors!!!
  prog::Stage stage = prog::Vertex;

  if ( ! fnames.empty())
    pProg->setName(fnames[ 0 ]);

  for ( auto fname : fnames )
  {
    std::string out;
    
    if ( mSearchPath.resolve(fname, fname))
    {
      if (!readFile(fname, out))
      {
        PNIDBGSTR("failed to read file " << fname);
        return nullptr;
      }

      pProg->setProgStr(stage, out);
      stage = ( prog::Stage ) ( stage + 1 );
    }
    else
    {
      PNIDBGSTR("could not resolve file for " << fname);
      return nullptr;
    }
  }
  
  return pProg.good();
}

progFactory::ProgFuture progFactory::loadAsync ( Filenames const& fnames )
{
  return mThreadPool.enqueue([=]() { return this->loadSync(fnames); });
}

void progFactory::cancel ( ProgFuture const& progFuture )
{
  // TODO: Implement cancel.
}

/////////////////////////////////////////////////////////////////////

} // end namespace namespacename


