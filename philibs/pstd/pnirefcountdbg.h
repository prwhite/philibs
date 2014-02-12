/////////////////////////////////////////////////////////////////////
//
//	class: refCount
//
//	This class is a simple class for intrusive reference counting.
//	Do not use this class if you are setting up reference counting
//  for a system of inherited classes that have differing reference
//	counting needs... i.e. classes that are likeley to need to 
//	override any of the methods of this class.  They are not virtual
//	and you are playing with fire if you override them.
//
/////////////////////////////////////////////////////////////////////

#ifndef pnirefcountdbg_h
#define pnirefcountdbg_h

/////////////////////////////////////////////////////////////////////

#include <iosfwd>
#include "pnirefcount.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {

  class refCount;

/////////////////////////////////////////////////////////////////////

class dbgRefCount 
{
  public:
    typedef refCount::Ref Ref;
    typedef refCount::Refs Refs;
  
    enum Opts
    {
      None = 0x00,
      Summary = 0x01,
      Details = 0x02,
      Exhaustive = 0x04
    };
  
    static void doDbg ( unsigned int mask = Summary );
    
      // Defaults to std::cout.
    static void setStr ( std::ostream* str );
    static std::ostream* getStr ();
    
    static void trackRef ( Ref* pRef );
    
    static void collectRefs ( Ref* pRef, Refs& refs );
    
    static void doDbg ( Ref* pRef );
    
    template< class Type >
    static void collectVecRefs ( Type& srcVec, Refs& refs )
        {
          auto end = srcVec.end ();
          for ( auto cur = srcVec.begin ();
              cur != end; ++cur )
          {
            refs.push_back ( cur->get () );
          }
        }
        
    template< class Type >
    static void collectMapSecondRefs ( Type& srcVec, Refs& refs )
        {
          auto end = srcVec.end ();
          for ( auto cur = srcVec.begin ();
              cur != end; ++cur )
          {
            refs.push_back ( cur->second.get () );
          }
        }
};

/////////////////////////////////////////////////////////////////////

	} // end of namespace pstd 
} // end of namespace pni 

#endif // pnirefcountdbg_h


