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

#ifndef pnirefcount_h
#define pnirefcount_h

/////////////////////////////////////////////////////////////////////

// #include "pnipstddef.h"
// #include "pnipstd.h"

#define PNIPSTDAPI

// I need a 64-bit integer
#ifdef _WIN32
   typedef __int64 int64_t;	// Define it from MSVC's internal type
   typedef int int32_t;
#else
   #include <cstdint>		// Use the C99 official header
#endif

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {
		
		class dbgRefCount;
		
/////////////////////////////////////////////////////////////////////

class PNIPSTDAPI refCount 
{
    friend class dbgRefCount;

	public:
		typedef int32_t int32;
    typedef refCount const Ref;
    typedef std::vector< Ref* > Refs;
		
		// zero refs on creation
		refCount ();
		// ref count is not copied
		refCount ( refCount const& rhs );
		// ref count is not assigned
		refCount& operator = ( refCount const& rhs );

		// reference count interface
		bool ref ( int32 howMany = 1 ) const;
		bool unref ( int32 howMany = 1 ) const;
		int32 getNumRefs () const;
		
	protected:
		virtual ~refCount ();
		
		virtual void collectRefs ( Refs& refs ) const = 0;
		
		  // For debugging.
		void doDbg () const;
		virtual void onDoDbg () const {}
		
	private:
		int32 mutable volatile refs;				
};

/////////////////////////////////////////////////////////////////////

	} // end of namespace pstd 
} // end of namespace pni 

#endif // pnirefcount_h


