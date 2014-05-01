/////////////////////////////////////////////////////////////////////
//
//	file: plane.C
//
/////////////////////////////////////////////////////////////////////

#include "pnibox3.h"
#include "pniplane.h"
#include "pnisphere.h"
#include "pniseg.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

// like setPts
plane::
plane ( const vec3& pt1, const vec3& pt2, const vec3& pt3 ) :
	normal ( vec3::NoInit )
{
	setPts ( pt1, pt2, pt3 );
}



// like setNormPt
plane::
plane ( const vec3& norm, const vec3& ptOnplane ) :
	normal ( vec3::NoInit )
{
	setNormPt ( norm, ptOnplane );
}


/////////////////////////////////////////////////////////////////////
// set methods
plane&
plane::
operator = ( const plane& orig )
{
	normal = orig.normal;
	offset = orig.offset;
	
	return *this;
}

/////////////////////////////////////////////////////////////////////
// equality methods
bool
plane::
operator == ( const ThisType& other ) const
{
	if ( other.normal == normal && other.offset == offset )
		return true;
	else
		return false;
}

bool
plane::
equal ( const ThisType& other, ValueType fuzz ) const
{
	if ( other.normal.equal ( normal, fuzz ) && 
		 TraitType::equal ( other.offset, offset, fuzz ) )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////
// emptiness methods

bool
plane::
isEmpty () const
{
	if ( normal.vec[ 0 ] == TraitType::zeroVal &&
			normal.vec[ 1 ] == TraitType::zeroVal &&
			normal.vec[ 2 ] == TraitType::zeroVal )
		return true;
	else
		return false;
}


/////////////////////////////////////////////////////////////////////
// set a plane from points or a point and a normal
// from the pfplane man page
//	   A pfplane represents an infinite 2D plane as a normal and a distance
//     offset from the origin in the normal direction.  A point on the plane
//     satisfies the equation normal dot (x, y, z) = offset.

void
plane::
setPts ( const vec3& pt1, const vec3& pt2, const vec3& pt3 )
{
	vec3 v1 ( pt1 );
	v1 -= pt2;
	
	vec3 v2 ( pt3 );
	v2 -= pt2;
	
	normal.cross ( v2, v1 );
	normal.normalize ();
	
	// the following code is a dup of setNormPt which doesn't redundantly
	// set the normal
	// setNormPt ( normal, pt1 );

	offset = normal.dot ( pt1 );
}


void
plane::
setNormPt ( const vec3& norm, const vec3& ptOnplane )
{
	offset = norm.dot ( ptOnplane );
	normal = norm;
}


/////////////////////////////////////////////////////////////////////
// simple displacement method
void
plane::
displace ( ValueType howFar )
{
	offset += howFar;
}


void
plane::
displace ( const vec3& pt )
{
	offset = normal.dot ( pt );
}


/////////////////////////////////////////////////////////////////////
// get the closest point to pt in the primitive
// the theory here is that backtracking 'dist'
// distance along the normal from the 'pt' will
// get us on the plane (verified)

// NON-OPTIMAL

bool
plane::
closestPtIn ( vec3& dest, ValueType xval, ValueType yval, ValueType zval ) const
{
	return closestPtIn ( dest, vec3 ( xval, yval, zval ) );
}

bool
plane::
closestPtIn ( vec3& dest, const vec3& pt ) const
{
	ValueType dist = -distToPtSigned ( pt );
	dest = normal;
	dest *= dist;
	dest += pt;
	
	return true;
}

/////////////////////////////////////////////////////////////////////
// distance to a point from the plane
plane::ValueType
plane::
distToPtSigned ( ValueType xval, ValueType yval, ValueType zval ) const
{
	// the code below is the expansion of the next line
	// return normal.dot ( vec3 ( xval, yval, zval ) ) - offset;

	return ( normal.vec[ 0 ] * xval + 
		     normal.vec[ 1 ] * yval +
		     normal.vec[ 2 ] * zval ) - offset;
}



// extendBy methods
void
plane::
extendBy ( const vec3& pt )
{
	if ( contains ( pt ) == containsResult::NoneIn )
	{
		ValueType d = distToPtSigned ( pt );
		displace ( d );
	}
}



void
plane::
extendBy ( const box3& box )
{
	if ( !box.isEmpty() )
	{
		vec3 a ( math::vec3::NoInit );
		vec3 b ( math::vec3::NoInit );
		box.get ( a, b );
		extendBy ( a );
		extendBy ( b );
	}
}



void
plane::
extendBy ( const sphere& sphereIn )
{
	if ( !sphereIn.isEmpty() )
	{
		vec3 pos ( math::vec3::NoInit );
		ValueType radius;
		sphereIn.get ( pos, radius );
		vec3 dir = normal;
		dir.normalize ();
		vec3 pt = pos + dir * radius;
		extendBy ( pt );
		pt = pos - dir * radius;
		extendBy ( pt );
	}
}




void
plane::
extendBy ( const seg& segIn )
{
	vec3 pos ( math::vec3::NoInit );
	vec3 dir ( math::vec3::NoInit );
	ValueType len;
	segIn.get ( pos, dir, len );
//	dir.normalize ();
//	dir = pos + dir * len;
	dir *= len;
	dir += pos;
	extendBy ( pos );
	extendBy ( dir );
}






/////////////////////////////////////////////////////////////////////
// contains methods
int
plane::
contains ( ValueType xval, ValueType yval, ValueType zval ) const
{
	if ( distToPtSigned ( xval, yval, zval ) >= 0.0f )
		return containsResult::SomeIn | containsResult::AllIn;
	else
		return containsResult::NoneIn;
}


// NON-OPTIMAL and APPROXIMATED

int
plane::
contains ( const box3& box ) const
{
	sphere sphere;
	sphere.extendBy ( box );

	// TODO: FIXME: Change this to be the check for containment of the vertices for the box3.
	// If all 8 vertices are contained  ==> containsResult::AllIn
	// If no vertices ==> containsResult::NoneIn
	// Otherwise ==> containsResult::SomeIn

	return contains ( sphere );
}



int
plane::
contains ( const sphere& sphere ) const
{
	ValueType dist = distToPtSigned ( sphere.center );
	
	if ( -dist > sphere.radius )
		return containsResult::NoneIn;
	else if ( dist > sphere.radius )
		return containsResult::SomeIn | containsResult::AllIn;
	else
		return containsResult::SomeIn;
}


int
plane::
contains ( const seg& segIn ) const
{
	vec3 pos ( math::vec3::NoInit );
	vec3 dir ( math::vec3::NoInit );
	ValueType len;
	segIn.get ( pos, dir, len );
//	dir.normalize ();
//	vec3 endPos = pos + dir * len;
	vec3 endPos ( dir );
	endPos *= len;
	endPos += pos;

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

	return containsResult::NoneIn;
}





int
plane::
contains ( const plane& planeIn ) const
{
	if ( this == &planeIn )
		return containsResult::AllIn | containsResult::SomeIn;

	// Planes must have the same normal and the "inside" plane/half-space must have a greater offset.
	ValueType dotVal = planeIn.normal.dot ( normal );

	if ( TraitType::equal ( dotVal, -1.0 ) )
	{
		if ( offset >= TraitType::abs ( planeIn.offset ) )
		{
			// "back to back", facing in opposite directions.
			return containsResult::NoneIn;
		}
		else
		{
			// Some overlap.
			return containsResult::SomeIn;
		}
	}

	if ( TraitType::equal ( dotVal, 1.0 ) &&
		 offset == planeIn.offset )
	{
		// Same plane/half-space specification.
		return containsResult::AllIn | containsResult::SomeIn;
	}

	return containsResult::SomeIn;
}





int
plane::
contains ( const cylinder& cyl ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}


  // From http://mathworld.wolfram.com/Reflection.html
void plane::mirror ( vec3& dest, vec3 const& src )
{
  ValueType d = distToPtSigned(src);
  
  dest = src - normal * 2.0f * d;
}

/////////////////////////////////////////////////////////////////////
// xform methods

void
plane::
xformOrtho ( const plane& planeIn, const matrix4& mat )
{
	vec3 ptOn ( vec3::NoInit );
	planeIn.closestPtIn ( ptOn, 0.0, 0.0, 0.0 );
	
	normal.xformVec ( planeIn.normal, mat );
	normal.normalize ();
	ptOn.xformPt ( ptOn, mat );
	
	// just like in setNormPt, but without redundant set of normal
	// setNormPt ( normal, ptOn )
	offset = normal.dot ( ptOn );
}



void
plane::
xformOrtho4 ( const plane& planeIn, const matrix4& mat )
{
	vec3 ptOn ( vec3::NoInit );
	planeIn.closestPtIn ( ptOn, 0.0, 0.0, 0.0 );
	
	normal.xformVec4 ( planeIn.normal, mat );
	normal.normalize ();
	ptOn.xformPt4 ( ptOn, mat );
	
	// just like in setNormPt, but without redundant set of normal
	// setNormPt ( normal, ptOn )
	offset = normal.dot ( ptOn );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

