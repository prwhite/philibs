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

/////////////////////////////////////////////////////////////////////

#ifndef PNIDBGDISABLE
  // Nada.
#else
#define PNIQUIET
#endif

#ifdef PNIQUIET
  #define PNIDBG
  #define PNIDBGSTR(str)
#else
  #ifndef _WIN32
    #define PNIDBG printf ( "%s:%d %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__ );
//    #define PNIDBGSTR(str) printf ( "%s:%d %s: %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, str );
    #define PNIDBGSTR(expr) std::cout << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << ": " << expr << std::endl;
  #else
    #define PNIDBG printf ( "%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__ );
    #define PNIDBGSTR(expr) std::cout << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": " << expr << std::endl
  #endif
#endif


/////////////////////////////////////////////////////////////////////

namespace pni {
    namespace pstd {
        
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

    } // end of namespace pstd 
} // end of namespace pni 

#endif // pnipnipstddbg_h


