/////////////////////////////////////////////////////////////////////
//
//	class: DoubleTrait
//
/////////////////////////////////////////////////////////////////////


#ifndef doubletrait_h
#define doubletrait_h

/////////////////////////////////////////////////////////////////////

#include <cmath>
#include "pnimath.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI DoubleTrait
{
	public:
		typedef double ValueType;
	
		static const ValueType zeroVal;
		static const ValueType oneVal;
		static const ValueType minVal;
		static const ValueType maxVal;
		static const ValueType fuzzVal;
		static const ValueType piDegVal;
		static const ValueType piRadVal;
		
		static ValueType abs ( ValueType valIn );
		static ValueType sqrt ( ValueType valIn );
		static ValueType sin ( ValueType valIn );
		static ValueType asin ( ValueType valIn );
		static ValueType cos ( ValueType valIn );
		static ValueType acos ( ValueType valIn );
		static ValueType tan ( ValueType valIn );
		static ValueType atan ( ValueType valIn );
		static ValueType atan2 ( ValueType xVal, ValueType yVal );
		static ValueType pow ( ValueType xVal, ValueType yVal );
		static ValueType d2r ( ValueType valIn );
		static ValueType r2d ( ValueType valIn );
		static bool equal ( ValueType aVal, ValueType bVal, ValueType fuzzIn = fuzzVal );

    static bool isNaN ( ValueType aVal );
    static bool isFinite ( ValueType aVal );
    static bool isInfinite ( ValueType aVal );
	
	private:
		DoubleTrait () {};
		DoubleTrait ( const DoubleTrait& ) {}
		~DoubleTrait () {}
};

/////////////////////////////////////////////////////////////////////

typedef DoubleTrait Trait;

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
abs ( ValueType valIn )
{
	return ::fabs ( valIn );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
sqrt ( ValueType valIn )
{
	return ::sqrt ( valIn );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
sin  ( ValueType valIn )
{
	return ::sin ( valIn );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
asin ( ValueType valIn )
{
	return ::asin ( valIn );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
cos  ( ValueType valIn )
{
	return ::cos ( valIn );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
acos ( ValueType valIn )
{
	return ::acos ( valIn );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
tan  ( ValueType valIn )
{
	return ::tan ( valIn );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
atan ( ValueType valIn )
{
	return ::atan ( valIn );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
atan2 ( ValueType yVal, ValueType xVal )
{
	return ::atan2 ( yVal, xVal );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
pow ( ValueType yVal, ValueType xVal )
{
	return ::pow ( yVal, xVal );
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
d2r  ( ValueType valIn )
{
  const float Mult = piRadVal / piDegVal;
	return Mult * valIn;
}

PNIMATHINLINE
DoubleTrait::ValueType
DoubleTrait::
r2d  ( ValueType valIn )
{
  const float Mult = piDegVal / piRadVal;
	return Mult * valIn;
}

PNIMATHINLINE
bool
DoubleTrait::
equal ( ValueType aVal, ValueType bVal, ValueType fuzzIn )
{
	if ( ( aVal + fuzzIn >= bVal ) && ( aVal - fuzzIn <= bVal ) )
		return true;
	else
		return false;
}

PNIMATHINLINE
bool
DoubleTrait::
isNaN ( ValueType aVal )
{
  return std::isnan ( aVal );
}

PNIMATHINLINE
bool
DoubleTrait::
isFinite ( ValueType aVal )
{
  return std::isfinite ( aVal );
}

PNIMATHINLINE
bool
DoubleTrait::
isInfinite ( ValueType aVal )
{
  return std::isinf ( aVal );
}


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // doubletrait_h
