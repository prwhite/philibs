/////////////////////////////////////////////////////////////////////
//
//	file: MathStream.C
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

#include "pnimathstream.h"

#include "pnibox3.h"
#include "pnicylinder.h"
#include "pnimatrix4.h"
#include "pniplane.h"
#include "pniseg.h"
#include "pnisphere.h"
#include "pnivec2.h"
#include "pnivec3.h"
#include "pnivec4.h"
#include "pnibox2.h"

/////////////////////////////////////////////////////////////////////
// box

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::box3& box )
{
	pni::math::vec3 tmp;

	box.getMin ( tmp );
	streamIn << tmp << PNIMATHSTD(endl);
	box.getMax ( tmp );
	streamIn << tmp << PNIMATHSTD(endl);
	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::box3& box )
{
	pni::math::vec3 tmin, tmax;

	streamIn >>	tmin;
	streamIn >> tmax;

	box.set ( tmin, tmax );

	return streamIn;
} 

/////////////////////////////////////////////////////////////////////
// box2

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::box2& box )
{
	pni::math::vec2 tmp;

	box.getMin ( tmp );
	streamIn << tmp << PNIMATHSTD(endl);
	box.getMax ( tmp );
	streamIn << tmp << PNIMATHSTD(endl);
	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::box2& box )
{
	pni::math::vec2 tmin, tmax;

	streamIn >>	tmin;
	streamIn >> tmax;

	box.set ( tmin, tmax );

	return streamIn;
} 


/////////////////////////////////////////////////////////////////////
// cylinder

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::cylinder& cyl )
{
	pni::math::vec3 tmp;
	pni::math::cylinder::ValueType ftmp;

	cyl.getCenter ( tmp );
	streamIn << tmp << PNIMATHSTD(endl);

	ftmp = cyl.getRadius ();
	streamIn << ftmp << PNIMATHSTD(endl);

	cyl.getAxis ( tmp );
	streamIn << tmp << PNIMATHSTD(endl);

	ftmp = cyl.getHalfLength ();
	streamIn << ftmp << PNIMATHSTD(endl);

	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::cylinder& cyl )
{
	pni::math::vec3 tmp;
	pni::math::cylinder::ValueType ftmp;

	streamIn >> tmp;
	cyl.setCenter ( tmp );

	streamIn >> ftmp;
	cyl.setRadius ( ftmp );

	streamIn >> tmp;
	cyl.setAxis ( tmp );

	streamIn >> ftmp;
	cyl.setHalfLength ( ftmp );

	return streamIn;
}

/////////////////////////////////////////////////////////////////////
// plane

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::plane& plane )
{
	pni::math::vec3 tmp;
	pni::math::plane::ValueType ftmp;

	plane.get ( tmp, ftmp );

	streamIn <<	tmp << PNIMATHSTD(endl) <<
			ftmp << PNIMATHSTD(endl);
	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::plane& plane )
{
	pni::math::vec3 tmp;
	pni::math::plane::ValueType ftmp;

	streamIn >> tmp >> ftmp;

	plane.set ( tmp, ftmp );

	return streamIn;
}

/////////////////////////////////////////////////////////////////////
// matrix4

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& str, const pni::math::matrix4& mat )
{
	pni::math::vec4 tmp;

	mat.getRow ( 0, tmp );
	str << tmp << PNIMATHSTD(endl);

	mat.getRow ( 1, tmp );
	str << tmp << PNIMATHSTD(endl);

	mat.getRow ( 2, tmp );
	str << tmp << PNIMATHSTD(endl);

	mat.getRow ( 3, tmp );
	str << tmp << PNIMATHSTD(endl);
		
	return str;
}

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& str, pni::math::matrix4& mat )
{
	pni::math::vec4 tmp;

	str >> tmp;
	mat.setRow ( 0, tmp );

	str >> tmp;
	mat.setRow ( 1, tmp );

	str >> tmp;
	mat.setRow ( 2, tmp );

	str >> tmp;
	mat.setRow ( 3, tmp );

	return str;
}


/////////////////////////////////////////////////////////////////////
// segment

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::seg& seg )
{
	pni::math::vec3 tmp;
	pni::math::seg::ValueType ftmp;

	seg.getPos ( tmp );
	streamIn << tmp << PNIMATHSTD(endl);

	seg.getPos ( tmp );
	ftmp = seg.getLength ();
	streamIn << tmp << PNIMATHSTD(endl) <<
			ftmp << PNIMATHSTD(endl);

	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::seg& seg )
{
	pni::math::vec3 tpos, tdir;
	pni::math::seg::ValueType tlen;

	streamIn >> tpos >> tdir >> tlen;
	seg.set ( tpos, tdir, tlen );

	return streamIn;
}

/////////////////////////////////////////////////////////////////////
// sphere

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::sphere& sphere )
{
	pni::math::vec3 tmp;
	pni::math::sphere::ValueType ftmp;

	sphere.get ( tmp, ftmp );

	streamIn <<	tmp << PNIMATHSTD(endl) <<
			ftmp << PNIMATHSTD(endl);
	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::sphere& sphere )
{
	pni::math::vec3 tmp;
	pni::math::sphere::ValueType ftmp;

	streamIn >> tmp >> ftmp;

	sphere.set ( tmp, ftmp );

	return streamIn;
}

/////////////////////////////////////////////////////////////////////
// vec2

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::vec2& vec )
{
	streamIn <<	vec[ 0 ] << ", " <<
				vec[ 1 ]; 
	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::vec2& vec )
{
	char buff[ 80 ];
	
	streamIn >>	vec[ 0 ] >> buff >>
				vec[ 1 ]; 
				
	return streamIn;
} 

/////////////////////////////////////////////////////////////////////
// vec3

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::vec3& vec )
{
	streamIn <<	vec[ 0 ] << ", " <<
				vec[ 1 ] << ", " <<
				vec[ 2 ]; 
	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::vec3& vec )
{
	char buff[ 80 ];
	
	streamIn >>	vec[ 0 ] >> buff >>
				vec[ 1 ] >> buff >>
				vec[ 2 ]; 
	return streamIn;
} 

/////////////////////////////////////////////////////////////////////
// vec4

PNIMATHSTD(ostream)&
operator << ( PNIMATHSTD(ostream)& streamIn, const pni::math::vec4& vec )
{
	streamIn <<	vec[ 0 ] << ", " <<
				vec[ 1 ] << ", " <<
				vec[ 2 ] << ", " <<
				vec[ 3 ]; 
	return streamIn;
} 

PNIMATHSTD(istream)&
operator >> ( PNIMATHSTD(istream)& streamIn, pni::math::vec4& vec )
{
	char buff[ 80 ];
	
	streamIn >>	vec[ 0 ] >> buff >>
				vec[ 1 ] >> buff >>
				vec[ 2 ] >> buff >>
				vec[ 3 ]; 
				
	return streamIn;
} 
