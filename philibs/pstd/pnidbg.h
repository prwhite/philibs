/////////////////////////////////////////////////////////////////////
//
//  class: dbg
//
/////////////////////////////////////////////////////////////////////

#ifndef pnipnipstddbg_h
#define pnipnipstddbg_h

/////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <iostream>
#include <sstream>

/////////////////////////////////////////////////////////////////////

#ifndef PNIDBGDISABLE // not disabling dbg
  #ifdef NDEBUG     // We are always quiet in release
    #define PNIQUIET
  #endif // NDEBUG
#else                 // disabling dbg
  #define PNIQUIET
#endif // PNIDBGDISABLE

#ifdef PNIQUIET
  #define PNIDBG
  #define PNIDBGSTR(str)
  #define PNIDBGSTR1T(str)
#else
  #define PNIDBG printf ( "%s:%d %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__ );
//  #define PNIDBGSTR(str) printf ( "%s:%d %s: %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, str );
  #define PNIDBGSTR(expr) std::cout << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << ": " << expr << std::endl;
  #define PNIDBGSTR1T(expr) { std::ostringstream pniostr; pniostr << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": " << expr; pnidbgstringonetime ( pniostr.str() ); }
  extern void pnidbgstringonetime ( std::string const& str );
#endif


/////////////////////////////////////////////////////////////////////

namespace pni {
    namespace pstd {
        
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

    } // end of namespace pstd 
} // end of namespace pni 

#endif // pnipnipstddbg_h


