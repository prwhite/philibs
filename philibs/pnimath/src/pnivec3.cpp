/////////////////////////////////////////////////////////////////////
//
//	vec3.C
//
/////////////////////////////////////////////////////////////////////

#include "pnivec3.h"
#include "pnivec4.h"		// for xform*4
#include "pniquat.h"
#include "pnimatrix4.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////

vec3::ValueType
vec3::
dist ( const vec3& vecIn ) const
{       
        return TraitType::sqrt ( 
                ( vec[ 0 ] - vecIn.vec[ 0 ] ) * ( vec[ 0 ] - vecIn.vec[ 0 ] ) +
                ( vec[ 1 ] - vecIn.vec[ 1 ] ) * ( vec[ 1 ] - vecIn.vec[ 1 ] ) +
                ( vec[ 2 ] - vecIn.vec[ 2 ] ) * ( vec[ 2 ] - vecIn.vec[ 2 ] ) );
}

vec3::ValueType
vec3::
length () const
{       
        return TraitType::sqrt ( 
                vec[ 0 ] * vec[ 0 ] +
                vec[ 1 ] * vec[ 1 ] +
                vec[ 2 ] * vec[ 2 ] );
}

void
vec3::
xformVec ( const vec3& src, 
		const matrix4& mat )
{
//	matrix4 mat ( matIn );
//	mat.invert ();
//	mat.transpose ();

	set (	src[ 0 ] * mat[ 0 ][ 0 ] +
			src[ 1 ] * mat[ 1 ][ 0 ] +
			src[ 2 ] * mat[ 2 ][ 0 ], 
			
			src[ 0 ] * mat[ 0 ][ 1 ] +
			src[ 1 ] * mat[ 1 ][ 1 ] +
			src[ 2 ] * mat[ 2 ][ 1 ], 
			
			src[ 0 ] * mat[ 0 ][ 2 ] +
			src[ 1 ] * mat[ 1 ][ 2 ] +
			src[ 2 ] * mat[ 2 ][ 2 ] );
}

// NON-OPTIMAL

void
vec3::
xformVec4 ( const vec3& src, const matrix4& mat )
{
#ifdef TOTALLYBOGUSXFORMVEC4
	//vec4 vec4 ( src.vec[ 0 ], src.vec[ 1 ], src.vec[ 2 ], TraitType::oneVal );
	//vec4.xformVec ( vec4, mat );
	//vec4 /= vec4.vec[ 3 ];
	//set ( vec4.vec[ 0 ], vec4.vec[ 1 ], vec4.vec[ 2 ] );
#endif // TOTALLYBOGUSXFORMVEC4

	vec4 vec4 ( src.vec[ 0 ], src.vec[ 1 ], src.vec[ 2 ], TraitType::zeroVal );
	vec4.xformVec ( vec4, mat );
	//vec4 /= vec4.vec[ 3 ];
	set ( vec4.vec[ 0 ], vec4.vec[ 1 ], vec4.vec[ 2 ] );
}

void
vec3::
xformPt ( const vec3& src, const matrix4& mat )
{
	set (	src[ 0 ] * mat[ 0 ][ 0 ] +
			src[ 1 ] * mat[ 1 ][ 0 ] +
			src[ 2 ] * mat[ 2 ][ 0 ] +
					   mat[ 3 ][ 0 ], 
			
			src[ 0 ] * mat[ 0 ][ 1 ] +
			src[ 1 ] * mat[ 1 ][ 1 ] +
			src[ 2 ] * mat[ 2 ][ 1 ] +
					   mat[ 3 ][ 1 ], 
			
			src[ 0 ] * mat[ 0 ][ 2 ] +
			src[ 1 ] * mat[ 1 ][ 2 ] +
			src[ 2 ] * mat[ 2 ][ 2 ] +
					   mat[ 3 ][ 2 ] );
}

// NON-OPTIMAL

void
vec3::
xformPt4 ( const vec3& src, const matrix4& mat )
{
	vec4 vec4 ( src.vec[ 0 ], src.vec[ 1 ], src.vec[ 2 ], TraitType::oneVal );
	vec4.xformPt ( vec4, mat );
	vec4 /= vec4.vec[ 3 ];
	set ( vec4.vec[ 0 ], vec4.vec[ 1 ], vec4.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////
// 
// from http://www.cis.ohio-state.edu/~parent/book/Full.html :
// To rotate a vector, v, by a quaternion, q, treat the vector as [0,v] and: 
// v' = Rot(v) = q-1*v*q 

void
vec3::
xformVec ( const vec3& src, 
		const quat& quatIn )
{
	quat inv ( quatIn );
	inv.invert ();
	quat avec ( src[ 0 ], src[ 1 ], src[ 2 ], TraitType::zeroVal );
	
	avec = inv * avec * quatIn;
	
	vec[ 0 ] = avec[ 0 ];
	vec[ 1 ] = avec[ 1 ];
	vec[ 2 ] = avec[ 2 ];
}

/////////////////////////////////////////////////////////////////////

vec3::ValueType
vec3::
getAxisAngle ( const vec3& v1, const vec3& v2 )
{
	ValueType angle;
	
	if ( v1.equal ( v2 ) )
	{
		angle = TraitType::zeroVal;

		set ( TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal );

		if ( v1.equal ( *this ) || v1.equal ( *this * -TraitType::oneVal ) )
			set ( TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal );
		
		cross ( v1, *this );
		cross ( v1, *this );
	}
	if ( v1.equal ( v2 * -TraitType::oneVal ) )
	{
		angle = TraitType::piDegVal;
		set ( TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal );

		if ( v1.equal ( *this ) || v1.equal ( *this * -TraitType::oneVal ) )
			set ( TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal );
		
		cross ( v1, *this );
		cross ( v1, *this );
	}
	else
	{
		ValueType dotVal = v1.dot ( v2 );

		if ( dotVal > TraitType::oneVal )
			dotVal = TraitType::oneVal;
		else if ( dotVal < -TraitType::oneVal )
			dotVal = -TraitType::oneVal;
		angle = TraitType::r2d ( TraitType::acos ( dotVal ) );
		
		cross ( v1, v2 );
	}
	
	return angle;
}


void
vec3::
projectOnto ( const vec3& target )
{
	ValueType factor;

	factor = dot ( target ) / ( target.dot ( target ) );

	set ( factor * target[0], factor * target[1], factor * target[2] );
}


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

