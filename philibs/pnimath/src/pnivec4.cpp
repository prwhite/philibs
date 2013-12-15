/////////////////////////////////////////////////////////////////////
//
//	vec4.C
//
/////////////////////////////////////////////////////////////////////

#include "pnivec4.h"
#include "pniquat.h"
#include "pnimatrix4.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

void
vec4::
xformVec ( const vec4& src, const matrix4& mat )
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
			src[ 2 ] * mat[ 2 ][ 2 ], 

			src[ 0 ] * mat[ 0 ][ 3 ] +
			src[ 1 ] * mat[ 1 ][ 3 ] +
			src[ 2 ] * mat[ 2 ][ 3 ] +
			src[ 3 ] * mat[ 3 ][ 3 ] );
}

void
vec4::
xformPt ( const vec4& src, 
		const matrix4& mat )
{
	set (	src[ 0 ] * mat[ 0 ][ 0 ] +
			src[ 1 ] * mat[ 1 ][ 0 ] +
			src[ 2 ] * mat[ 2 ][ 0 ] +
			src[ 3 ] * mat[ 3 ][ 0 ], 
			
			src[ 0 ] * mat[ 0 ][ 1 ] +
			src[ 1 ] * mat[ 1 ][ 1 ] +
			src[ 2 ] * mat[ 2 ][ 1 ] +
			src[ 3 ] * mat[ 3 ][ 1 ], 
			
			src[ 0 ] * mat[ 0 ][ 2 ] +
			src[ 1 ] * mat[ 1 ][ 2 ] +
			src[ 2 ] * mat[ 2 ][ 2 ] +
			src[ 3 ] * mat[ 3 ][ 2 ], 

			src[ 0 ] * mat[ 0 ][ 3 ] +
			src[ 1 ] * mat[ 1 ][ 3 ] +
			src[ 2 ] * mat[ 2 ][ 3 ] +
			src[ 3 ] * mat[ 3 ][ 3 ] );
}

vec4::ValueType
vec4::
dist ( const vec4& vecIn ) const
{       
        return TraitType::sqrt ( 
                ( vec[ 0 ] - vecIn.vec[ 0 ] ) * ( vec[ 0 ] - vecIn.vec[ 0 ] ) +
                ( vec[ 1 ] - vecIn.vec[ 1 ] ) * ( vec[ 1 ] - vecIn.vec[ 1 ] ) +
                ( vec[ 2 ] - vecIn.vec[ 2 ] ) * ( vec[ 2 ] - vecIn.vec[ 2 ] ) +
                ( vec[ 3 ] - vecIn.vec[ 3 ] ) * ( vec[ 3 ] - vecIn.vec[ 3 ] ) );
}

vec4::ValueType
vec4::
length () const
{       
        return TraitType::sqrt ( 
                vec[ 0 ] * vec[ 0 ] +
                vec[ 1 ] * vec[ 1 ] +
                vec[ 2 ] * vec[ 2 ] +
                vec[ 3 ] * vec[ 3 ] );
}

/////////////////////////////////////////////////////////////////////

vec4::ValueType
vec4::
distHom ( const vec4& vecIn ) const
{
	vec3 th ( vec[ 0 ], vec[ 1 ], vec[ 2 ] );
	vec3 in ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );

	th /= vec[ 3 ];
	in /= vecIn.vec[ 3 ];
	
	return th.dist ( in );
}

vec4::ValueType
vec4::
lengthHom () const
{       
	vec3 th ( vec[ 0 ], vec[ 1 ], vec[ 2 ] );

	th /= vec[ 3 ];
	
	return th.length ();
}

vec4::ValueType
vec4::
distSqrHom ( const vec4& vecIn ) const
{
	vec3 th ( vec[ 0 ], vec[ 1 ], vec[ 2 ] );
	vec3 in ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );

	th /= vec[ 3 ];
	in /= vecIn.vec[ 3 ];

	return th.distSqr ( in );
}

/////////////////////////////////////////////////////////////////////
// 
// from http://www.cis.ohio-state.edu/~parent/book/Full.html :
// To rotate a vector, v, by a quaternion, q, treat the vector as [0,v] and: 
// v' = Rot(v) = q-1*v*q 
//

void
vec4::
xformVec ( const vec4& srcIn, 
		const quat& quatIn )
{
	quat inv ( quatIn );
	inv.invert ();
	
	ValueType divisor = srcIn[ 3 ];
	vec4 src ( srcIn );
	src /= divisor;			// get the vec out o.homogeneous coords

	quat avec ( src[ 0 ], src[ 1 ], src[ 2 ], TraitType::zeroVal );
	
	avec = inv * avec * quatIn;
	
	vec[ 0 ] = avec[ 0 ];
	vec[ 1 ] = avec[ 1 ];
	vec[ 2 ] = avec[ 2 ];
	vec[ 3 ] = TraitType::oneVal;
	
	*this *= divisor;
}



void
vec4::
projectOnto ( const ThisType& target )
{
	ValueType factor;

	factor = dot ( target ) / ( target.dot ( target ) );

	set ( factor * target[0], factor * target[1], factor * target[2], factor * target[3] );
}


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni
