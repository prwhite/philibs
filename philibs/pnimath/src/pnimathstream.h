/////////////////////////////////////////////////////////////////////
//
//	class: MathStream
//
//	This file introduces a bunch of global iostream operators
//	for this libraries math types.
//
/////////////////////////////////////////////////////////////////////


#ifndef pnimathstream_h
#define pnimathstream_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnimathios.h"

/////////////////////////////////////////////////////////////////////

// forward declare of math classes
namespace pni {
	namespace math {
		class box3;
		class box2;
		class cylinder;
		class matrix4;
		class plane;
		class seg;
		class sphere;
		class vec2;
		class vec3;
		class vec4;
	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////
// box

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::box3& box );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::box3& box );


/////////////////////////////////////////////////////////////////////
// box2

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::box2& box );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::box2& box );

/////////////////////////////////////////////////////////////////////
// cylinder

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::cylinder& cyl );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::cylinder& cyl );

/////////////////////////////////////////////////////////////////////
// plane

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::plane& plane );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, const pni::math::plane& plane );

/////////////////////////////////////////////////////////////////////
// matrix4

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::matrix4& mat );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::matrix4& mat );

/////////////////////////////////////////////////////////////////////
// segment

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::seg& seg );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::seg& seg );

/////////////////////////////////////////////////////////////////////
// sphere

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::sphere& sphere );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::sphere& sphere );

/////////////////////////////////////////////////////////////////////
// vec2

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::vec2& vec );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::vec2& vec );

/////////////////////////////////////////////////////////////////////
// vec3

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::vec3& vec );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::vec3& vec );

/////////////////////////////////////////////////////////////////////
// vec4 and quat

PNIMATHAPI
PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::vec4& vec );

PNIMATHAPI
PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::vec4& vec );

/////////////////////////////////////////////////////////////////////

#endif // pnimathstream_h
