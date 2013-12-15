/////////////////////////////////////////////////////////////////////
//
//	class: FloatTrait
//
/////////////////////////////////////////////////////////////////////


#ifndef pnimathfp_h
#define pnimathfp_h

/////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cfloat>
#include "pnimath.h"

#include "pnimatrix4.h"
#include "pnivec3.h"
#include "pnivec4.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

inline
bool isNumber(float x) 
{
    // This looks like it should always be true, but it's not if x is a NaN.
    return (x == x); 
}

inline
bool isFiniteNumber(float x) 
{
    return (x <= FLT_MAX && x >= -FLT_MAX); 
}

inline
bool isFpGood ( float x )
{
  return isNumber ( x ) && isFiniteNumber ( x );
}

inline
bool isFpGood ( pni::math::vec3 const& vec )
{
  return isFpGood ( vec[ 0 ] ) &&
      isFpGood ( vec[ 1 ] ) &&
      isFpGood ( vec[ 2 ] );
}

inline
bool isFpGood ( pni::math::vec4 const& vec )
{
  return isFpGood ( vec[ 0 ] ) &&
      isFpGood ( vec[ 1 ] ) &&
      isFpGood ( vec[ 2 ] ) &&
      isFpGood ( vec[ 3 ] );
}

inline
bool isFpGood ( pni::math::matrix4 const& mat )
{
  bool retVal = true;
  
  pni::math::vec4 tmp;
  
  mat.getRow ( 0, tmp );
  retVal &= isFpGood ( tmp );

  mat.getRow ( 1, tmp );
  retVal &= isFpGood ( tmp );

  mat.getRow ( 2, tmp );
  retVal &= isFpGood ( tmp );

  mat.getRow ( 3, tmp );
  retVal &= isFpGood ( tmp );

  return retVal;
}

inline
bool isFpGood ( scene::matrix4 const& mat )
{
  pni::math::matrix4 const& tmp = mat;
  return isFpGood ( tmp );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#ifdef PNIFPGOODDISABLE
#define PNIFPGOOD(val)
#else
#define PNIFPGOOD(val) assert(pni::math::isFpGood(val))
#endif

/////////////////////////////////////////////////////////////////////

#endif // pnimathfp_h
