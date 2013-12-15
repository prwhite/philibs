/////////////////////////////////////////////////////////////////////
//
//	class: FloatTrait
//
/////////////////////////////////////////////////////////////////////


#ifndef pnifloattrait_h
#define pnifloattrait_h

/////////////////////////////////////////////////////////////////////

#include <cmath>
#include "pnimath.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI FloatTrait
{
	public:
			//mtcl: void mtcl_abstract();
		typedef float ValueType;
	
		static const ValueType zeroVal;
		static const ValueType oneVal;
		static const ValueType minVal;
		static const ValueType maxVal;
		static const ValueType fuzzVal;
		static const ValueType piDegVal;
		static const ValueType piRadVal;
		
		static ValueType abs ( ValueType valIn );
		static ValueType sqrt ( ValueType valIn );
		static ValueType fastInvSqrt ( ValueType valIn, int iter = 2 );
		static ValueType sin ( ValueType valIn );
		static ValueType asin ( ValueType valIn );
		static ValueType cos ( ValueType valIn );
		static ValueType acos ( ValueType valIn );
		static ValueType tan ( ValueType valIn );
		static ValueType atan ( ValueType valIn );
		static ValueType atan2 ( ValueType xVal, ValueType yVal );
		static ValueType pow ( ValueType xVal, ValueType yVal );
		static ValueType fmod ( ValueType valIn, ValueType modIn );
		static ValueType d2r ( ValueType valIn );
		static ValueType r2d ( ValueType valIn );
		static bool equal ( ValueType aVal, ValueType bVal, ValueType fuzzIn = fuzzVal );
	
	private:
		FloatTrait () {};
		FloatTrait ( const FloatTrait& ) {}
		~FloatTrait () {}
};

/////////////////////////////////////////////////////////////////////

typedef FloatTrait Trait;

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
abs ( ValueType valIn )
{
	return ::fabsf ( valIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
sqrt ( ValueType valIn )
{
	return ::sqrtf ( valIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
sin  ( ValueType valIn )
{
	return ::sinf ( valIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
asin ( ValueType valIn )
{
	return ::asinf ( valIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
cos  ( ValueType valIn )
{
	return ::cosf ( valIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
acos ( ValueType valIn )
{
	return ::acosf ( valIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
tan  ( ValueType valIn )
{
	return ::tanf ( valIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
atan ( ValueType valIn )
{
	return ::atanf ( valIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
atan2 ( ValueType yVal, ValueType xVal )
{
	return ::atan2f ( yVal, xVal );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
pow ( ValueType yVal, ValueType xVal )
{
	return ::powf ( yVal, xVal );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
fmod  ( ValueType valIn, ValueType modIn )
{
	return ::fmodf ( valIn, modIn );
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
d2r  ( ValueType valIn )
{
	return piRadVal / piDegVal * valIn;
}

PNIMATHINLINE
FloatTrait::ValueType
FloatTrait::
r2d  ( ValueType valIn )
{
	return piDegVal / piRadVal * valIn;
}

PNIMATHINLINE
bool
FloatTrait::
equal ( ValueType aVal, ValueType bVal, ValueType fuzzIn )
{
	if ( ( aVal + fuzzIn >= bVal ) && ( aVal - fuzzIn <= bVal ) )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnifloattrait_h
