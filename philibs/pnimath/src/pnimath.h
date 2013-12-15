/////////////////////////////////////////////////////////////////////
//
//	Math.h
//
/////////////////////////////////////////////////////////////////////


#ifndef pnimath_h
#define pnimath_h

/////////////////////////////////////////////////////////////////////

#include <cstdlib>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

// machine/compiler/build dependencies

#ifdef __BEOS__
	#define PNIMATHANSIDRAFT
	#define PNIMATHHASDOUBLE
	#define PNIMATHNEEDINLINES
	#ifdef __intel
		#define PNIMATHDODECLSPEC
	#endif
#endif

#ifdef __APPLE__
	#define PNIMATHHASFLOAT
	#define PNIMATHHASDOUBLE
	#define PNIMATHNAMESPACES
	#define PNIMATHANSIDRAFT
	#define PNIMATHNEEDINLINES
#endif

#ifdef __sgi
	#define PNIMATHHASFLOAT
	#define PNIMATHHASDOUBLE
	#define PNIMATHSGINONANSIDRAFT
	#if __EDG_ABI_COMPATIBILITY_VERSION<=227 || ! _BOOL
		#define PNIMATHNEEDBOOL
	#endif
#endif

#ifdef __linux__
	#define PNIMATHHASFLOAT
	#define PNIMATHHASDOUBLE
	#define PNIMATHNAMESPACES
	#define PNIMATHANSIDRAFT
	#define PNIMATHNEEDINLINES
#endif

#ifdef _WIN32
	#define PNIMATHHASFLOAT
	#define PNIMATHHASDOUBLE
	#define PNIMATHNAMESPACES
	#define PNIMATHANSIDRAFT
	#define PNIMATHDODECLSPEC
	#define PNIMATHNEEDINLINES
#endif

// debug support
#ifdef PNIMATHDEBUG
	#ifdef PNIMATHNEEDINLINES
		#define PNIMATHINLINE inline
	#else
		#define PNIMATHINLINE
	#endif
	
	#include "pnimathios.h"

	#define PNIMATHUNIMPLEMENTED PNIMATHSTD(cerr) << "unimplemented: " << \
		__file__ << ":" << __line__ << PNIMATHSTD(endl); exit ( -1 )

	#define PNIMATHUNTESTED { \
		static int done = 0; \
		if ( ! done ) \
			PNIMATHSTD(cerr) << "untested: " << __file__ << ":" << __line__ << PNIMATHSTD(endl); \
			done = 1; \
		}
#else
	#define PNIMATHINLINE inline
	#define PNIMATHUNIMPLEMENTED
	#define PNIMATHUNTESTED
#endif

// stdlib compat
#ifdef PNIMATHANSIDRAFT
	#define PNIMATHUSING(what) using what;
	#define PNIMATHSTD(what) std::what
#else
	#define PNIMATHUSING(what)
	#define PNIMATHSTD(what) what
#endif

// namespace support
#ifdef PNIMATHNAMESPACES
	#define PNIMATHSTART namespace Math
	#define PNIMATHEND }
#else
	#define PNIMATHSTART
	#define PNIMATHEND
#endif

#include "pnimathdef.h"

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

// crappy test for existence of bool
#ifdef PNIMATHNEEDBOOL
	enum bool { false, true };
#endif

// for conditional double/float compilation

class box3;
class cylinder;
class matrix4;
class plane;
class quat;
class seg;
class sphere;
class vec2;
class vec3;
class vec4;
template< class Type > class matStack;

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#ifdef PNIMATHDOUBLE
	#include "pnidoubletrait.h"
	#include "pnidoublemath.h"
#else
	#include "pnifloattrait.h"
	#include "pnifloatmath.h"
#endif

/////////////////////////////////////////////////////////////////////

#endif // pnimath_h
