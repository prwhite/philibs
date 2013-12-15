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

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {

  class refCount;

/////////////////////////////////////////////////////////////////////

class dbgRefCount 
{
  public:
    typedef std::vector< refCount* > Refs;
  
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
    
    static void trackRef ( refCount* pRef );
    
    static void collectRefs ( refCount* pRef, Refs& refs );
    
    static void doDbg ( refCount* pRef );
    
    template< class Type >
    static void collectVecRefs ( Type& srcVec, Refs& refs )
        {
          typename Type::iterator end = srcVec.end ();
          for ( typename Type::iterator cur = srcVec.begin ();
              cur != end; ++cur )
          {
            refs.push_back ( cur->get () );
          }
        }
        
    template< class Type >
    static void collectMapSecondRefs ( Type& srcVec, Refs& refs )
        {
          typename Type::iterator end = srcVec.end ();
          for ( typename Type::iterator cur = srcVec.begin ();
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


