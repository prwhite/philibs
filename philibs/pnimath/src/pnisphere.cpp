/////////////////////////////////////////////////////////////////////
//
//	file: sphere.C
//
/////////////////////////////////////////////////////////////////////

#include "pnisphere.h"
#include "pnibox3.h"
#include "pnicylinder.h"
#include "pniseg.h"
#include "pnicontains.h"

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////
// equality methods
bool
sphere::
operator == ( const ThisType& other ) const
{
	if ( other.center == center && other.radius == radius )
		return true;
	else
		return false;
}

bool
sphere::
equal ( const ThisType& other, ValueType fuzz ) const
{
	if ( other.center.equal ( center, fuzz ) &&
		 TraitType::equal ( other.radius, radius, fuzz ) )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////
// empty methods

bool 
sphere::
isEmpty () const
{
	if ( radius < TraitType::zeroVal )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////
// contains methods

inline static
sphere::ValueType
sqrValue ( sphere::ValueType val )
{
	return val * val;
}




int
sphere::
contains ( ValueType xval, ValueType yval, ValueType zval ) const
{
	PNIMATHUNTESTED;

	if ( ( sqrValue ( xval - center.vec[ 0 ] ) +
	       sqrValue ( yval - center.vec[ 1 ] ) +
		   sqrValue ( zval - center.vec[ 2 ] ) ) <= sqrValue ( radius ) )
		return containsResult::SomeIn | containsResult::AllIn;
	else
		return containsResult::NoneIn;
}



int
sphere::
contains ( const sphere& sphere ) const
{
	PNIMATHUNTESTED;

	int retVal = containsResult::NoneIn;
	
	ValueType distSqr = center.distSqr ( sphere.center );
	ValueType r1Sqr = radius * radius;
	ValueType r2Sqr = sphere.radius * sphere.radius;
	
	if ( distSqr <= r1Sqr + r2Sqr )
	{
		retVal = containsResult::SomeIn;
		
		if ( r1Sqr >= distSqr + r2Sqr )
			retVal = containsResult::AllIn | containsResult::SomeIn;
	}

	return retVal;
}



/////////////////////////////////////////////////////////////////////
// from Graphics Gems I: p 335 "A Simple Method for Box-sphere
//  Intersection Testing"
// NON-OPTIMAL

int
sphere::
contains ( const box3& box ) const
{
	PNIMATHUNTESTED;
	
	ValueType distSqr = TraitType::zeroVal;
	
	// unroll this loop someday
	// note, a very similar, yet different variation of this exists
	// in box3::closestPtIn, but does more work than we need here
	for ( int num = 0; num < 3; num++ )
	{
		if ( center.vec[ num ] < box.minPos[ num ] )
		{
			ValueType diff = center.vec[ num ] - box.minPos[ num ];
			distSqr += diff * diff;
		}
		else if ( center.vec[ num ] > box.maxPos[ num ] )
		{
			ValueType diff = center.vec[ num ] - box.maxPos[ num ];
			distSqr += diff * diff;
		}
	}
	
	// beware, this method does not yet support the containsResult::AllIn return value
	if ( distSqr <= radius * radius )
		return containsResult::SomeIn;
	else
		return containsResult::NoneIn;
}



// OPTIMIZE: This is a HUGE/heavyweight function.  Probably several ways to optimize it.
int
sphere::
contains ( const seg& segIn ) const
{
	vec3 pos;
	vec3 dir;
	ValueType len;
	segIn.get ( pos, dir, len );
	dir.normalize ();
	vec3 endPos = pos + dir * len;

	// The segment begin and endpoint might lie in the sphere.
	int c1 = contains ( pos );
	int c2 = contains ( endPos );

	if ( c1 )
	{
		if ( c2 )
			return containsResult::AllIn | containsResult::SomeIn;

		return containsResult::SomeIn;
	}

	if ( c2 )
	{
		return containsResult::SomeIn;
	}



	ValueType a = dir.dot ( dir );
	//ValueType a = dir.vec[0] * dir.vec[0] + dir.vec[1] * dir.vec[1] + dir.vec[2] * dir.vec[2];
	ValueType b = static_cast< ValueType > ( 2.0 ) * dir.vec[0] * ( pos.vec[0] - center.vec[0] ) +
	              static_cast< ValueType > ( 2.0 ) * dir.vec[1] * ( pos.vec[1] - center.vec[1] ) +
	              static_cast< ValueType > ( 2.0 ) * dir.vec[2] * ( pos.vec[2] - center.vec[2] );
	ValueType tx = pos.vec[0] - center.vec[0];
	ValueType ty = pos.vec[1] - center.vec[1];
	ValueType tz = pos.vec[2] - center.vec[2];
	ValueType c = tx * tx + ty * ty + tz * tz - radius * radius;
	//ValueType c = ( pos.vec[0] - center.vec[0] ) * ( pos.vec[0] - center.vec[0] ) +
	//			  ( pos.vec[1] - center.vec[1] ) * ( pos.vec[0] - center.vec[0] ) +
	//			  ( pos.vec[0] - center.vec[0] ) * ( pos.vec[0] - center.vec[0] ) -
	//			  radius * radius;

	// Check the discriminant to see if there are any solutions.
	ValueType discriminant = b * b - static_cast< ValueType > ( 4.0 ) * a * c;
	if ( discriminant < TraitType::zeroVal )
	{
		// No solutions --> no intersections.
		return containsResult::NoneIn;
	}

	ValueType twoA = static_cast< ValueType > ( 2.0 ) * a;

	if ( discriminant == TraitType::zeroVal )
	{
		// One intersection.  Segment is tangential.
		ValueType t1 = -b / twoA;
		vec3 ip1 = pos + dir * t1;

		return segIn.contains ( ip1 );
	}

	// Two intersections.
	ValueType t1 = ( -b + TraitType::sqrt ( discriminant ) ) / twoA;
	ValueType t2 = ( -b - TraitType::sqrt ( discriminant ) ) / twoA;
	vec3 ip1 = pos + dir * t1;
	vec3 ip2 = pos + dir * t2;

	if ( segIn.contains ( ip1 ) || segIn.contains ( ip2 ) )
		return containsResult::SomeIn;
	else
		return containsResult::NoneIn;
}





int
sphere::
contains ( const cylinder& cyl ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}



/////////////////////////////////////////////////////////////////////
// get the closest point to pt in the primative
bool
sphere::
closestPtIn ( vec3& dest, ValueType xval, ValueType yval, ValueType zval ) const
{
	if ( isEmpty () )
		return false;

	if ( contains ( xval, yval, zval ) )
		dest.set ( xval, yval, zval );
	else
	{
		dest.set ( xval, yval, zval );
		dest -= center;
		dest.normalize ();
		dest *= radius;
		dest += center;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////
// extend by methods
// Graphics Gems I: p 301 "An Efficient Bounding sphere"

// NON-OPTIMAL

void
sphere::
extendBy ( ValueType xval, ValueType yval, ValueType zval )
{
	extendBy ( vec3 ( xval, yval, zval ) );
}

void
sphere::
extendBy ( const vec3& pt )
{
	PNIMATHUNTESTED;

	if ( isEmpty () )
	{
		center = pt;
		radius = TraitType::zeroVal;		// or should this be epsilon?
	}
	
	ValueType dist = center.dist ( pt );

	if ( dist > radius )
	{
		radius = ( radius + dist ) / ( ValueType ) 2.0;
		
		center -= pt;				// diff vector pointing towards old center
		center *= radius / dist;	// normalize vector, multiply by radius
		center += pt;				// offset from pt towards center
	}
}

// NON-OPTIMAL

void
sphere::
extendBy ( const sphere& sphere )
{
	PNIMATHUNTESTED;

	if ( this == &sphere )
		return;					// no extend by self.

	if ( sphere.isEmpty () )
		return;					// do nothing with empty spheres

	if ( isEmpty () )
	{
		*this = sphere;
	}
	else
	{
		// this finds the vector seperating this and sphere and add to
		// the length the radius of the other sphere to get a point on
		// the far side of sphere... it then calls extendBy with the
		// calculated point

		vec3 diff ( sphere.center );
		diff -= center;

		ValueType diffLen = diff.length ();

		diff /= diffLen;

		diffLen += sphere.radius;

		diff *= diffLen;

		diff += center;

		extendBy ( diff );
	}
}

// NON-OPTIMAL

void
sphere::
extendBy ( const box3& box )
{
	PNIMATHUNTESTED;

	if ( !box.isEmpty() )
	{
		sphere tmpsphere;
		tmpsphere.extendBy ( box.minPos );
		tmpsphere.extendBy ( box.maxPos );
		
		extendBy ( tmpsphere );
	}
}

void
sphere::
extendBy ( const cylinder& cyl )
{
	PNIMATHUNIMPLEMENTED;
}

/////////////////////////////////////////////////////////////////////
// xform method

void
sphere::
xformOrtho ( const sphere& sphere, const matrix4& mat )
{
	PNIMATHUNTESTED;

	if ( isEmpty () )
		return;

	vec3 tmp ( vec3::NoInit );	
	tmp.set ( sphere.radius, TraitType::zeroVal, TraitType::zeroVal );
	tmp.xformVec ( tmp, mat );
	radius = tmp.length ();
	
	center.xformPt ( sphere.center, mat );
}


void
sphere::
xformOrtho4 ( const sphere& sphere, const matrix4& mat )
{
	PNIMATHUNTESTED;

	if ( isEmpty () )
		return;

	vec3 tmp ( vec3::NoInit );	
	tmp.set ( sphere.radius, TraitType::zeroVal, TraitType::zeroVal );
	tmp.xformVec4 ( tmp, mat );
	radius = tmp.length ();
	
	center.xformPt4 ( sphere.center, mat );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni
