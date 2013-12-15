/////////////////////////////////////////////////////////////////////
//
//	DoubleTrait.C
//
/////////////////////////////////////////////////////////////////////

#include "pnidoubletrait.h"
#include <cfloat>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

const DoubleTrait::ValueType DoubleTrait::zeroVal = 0.0;
const DoubleTrait::ValueType DoubleTrait::oneVal = 1.0;
const DoubleTrait::ValueType DoubleTrait::minVal = DBL_MIN;
const DoubleTrait::ValueType DoubleTrait::maxVal = DBL_MAX;
const DoubleTrait::ValueType DoubleTrait::fuzzVal = DBL_EPSILON;
const DoubleTrait::ValueType DoubleTrait::piDegVal = 180.0;
const DoubleTrait::ValueType DoubleTrait::piRadVal = M_PI;

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

