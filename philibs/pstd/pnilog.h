/////////////////////////////////////////////////////////////////////
//
//  
//
/////////////////////////////////////////////////////////////////////

#ifndef pnipstdlog_h
#define pnipstdlog_h

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

#include "pnitimer.h"

#include <stdio.h>

/////////////////////////////////////////////////////////////////////

namespace pni {
  namespace pstd {
  
pni::pstd::float64 getBaseLogTime ();
pni::pstd::float64 getDeltaLogTime (); 

  } // end namespace pstd
} // end namespace pni

#ifndef PNIPSTDLOGDISABLE
#define PNIPSTDLOGENABLE
#endif

#ifdef PNIPSTDLOGENABLE

  #ifdef WIN32

    #define PNIPSTDLOG printf ( "%0#10.6f : %s:%d %s\n", \
        pni::pstd::getBaseLogTime (), __FILE__, __LINE__, __FUNCTION__ );

    #define PNIPSTDLOGM(msg) printf ( "%0#10.6f : %s:%d %s : %s\n", \
        pni::pstd::getBaseLogTime (), __FILE__, __LINE__, __FUNCTION__, msg );

  #else // WIN32 -> Unixy

    #define PNIPSTDLOG printf ( "%0#10.6f : %s:%d %s\n", \
        pni::pstd::getBaseLogTime (), __FILE__, __LINE__, __PRETTY_FUNCTION__ );

    #define PNIPSTDLOGM(msg) printf ( "%0#10.6f : %s:%d %s : %s\n", \
        pni::pstd::getBaseLogTime (), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg );

  #endif // WIN32

#else

#define PNIPSTDLOG
#define PNIPSTDLOGM(msg)

#endif // PNIPSTDLOGENABLE

/////////////////////////////////////////////////////////////////////

#endif // pnipstdlog_h
