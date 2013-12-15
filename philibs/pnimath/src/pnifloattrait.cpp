/////////////////////////////////////////////////////////////////////
//
//	FloatTrait.C
//
/////////////////////////////////////////////////////////////////////

#include "pnifloattrait.h"
#include <cfloat>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

const FloatTrait::ValueType FloatTrait::zeroVal = 0.0f;
const FloatTrait::ValueType FloatTrait::oneVal = 1.0f;
const FloatTrait::ValueType FloatTrait::minVal = FLT_MIN;
const FloatTrait::ValueType FloatTrait::maxVal = FLT_MAX;
const FloatTrait::ValueType FloatTrait::fuzzVal = FLT_EPSILON;
const FloatTrait::ValueType FloatTrait::piDegVal = 180.0f;
const FloatTrait::ValueType FloatTrait::piRadVal = ( float ) M_PI;

/////////////////////////////////////////////////////////////////////

  // from http://betterexplained.com/articles/understanding-quakes-fast-inverse-square-root/http://betterexplained.com/articles/understanding-quakes-fast-inverse-square-root/
  // or http://www.hackszine.com/blog/archive/2008/12/quakes_fast_inverse_square_roo.html?CMP=OTC-7G2N43923558

FloatTrait::ValueType
FloatTrait::
fastInvSqrt ( ValueType valIn, int iter )
{
  float xhalf = 0.5f*valIn; 
  int i = *(int*)&valIn; // get bits for floating value 
  i = 0x5f3759df - (i>>1); // gives initial guess y0 
  valIn = *(float*)&i; // convert bits back to float 

  switch ( iter )
  {
    case 8:
      valIn = valIn*(1.5f-xhalf*valIn*valIn); // Newton step, repeating increases accuracy 
    case 7:
      valIn = valIn*(1.5f-xhalf*valIn*valIn); // Newton step, repeating increases accuracy 
    case 6:
      valIn = valIn*(1.5f-xhalf*valIn*valIn); // Newton step, repeating increases accuracy 
    case 5:
      valIn = valIn*(1.5f-xhalf*valIn*valIn); // Newton step, repeating increases accuracy 
    case 4:
      valIn = valIn*(1.5f-xhalf*valIn*valIn); // Newton step, repeating increases accuracy 
    case 3:
      valIn = valIn*(1.5f-xhalf*valIn*valIn); // Newton step, repeating increases accuracy 
    case 2:
      valIn = valIn*(1.5f-xhalf*valIn*valIn); // Newton step, repeating increases accuracy 
    case 1:
      valIn = valIn*(1.5f-xhalf*valIn*valIn); // Newton step, repeating increases accuracy 
    break;
  }

  return valIn; 
}


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni
