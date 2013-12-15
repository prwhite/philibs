/////////////////////////////////////////////////////////////////////
//
//	vec2.C
//
/////////////////////////////////////////////////////////////////////

#include "pnivec2.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////

vec2::ValueType
vec2::
dist ( const vec2& vecIn ) const
{       
        return TraitType::sqrt ( 
                ( vec[ 0 ] - vecIn.vec[ 0 ] ) * ( vec[ 0 ] - vecIn.vec[ 0 ] ) +
                ( vec[ 1 ] - vecIn.vec[ 1 ] ) * ( vec[ 1 ] - vecIn.vec[ 1 ] ) );
}

vec2::ValueType
vec2::
length () const
{       
        return TraitType::sqrt ( 
                vec[ 0 ] * vec[ 0 ] +
                vec[ 1 ] * vec[ 1 ] );
}


void
vec2::
projectOnto ( const ThisType& target )
{
	ValueType factor;

	factor = dot ( target ) / ( target.dot ( target ) );

	set ( factor * target[0], factor * target[1] );
}


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni
