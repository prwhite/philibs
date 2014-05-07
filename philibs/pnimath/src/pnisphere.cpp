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

// From http://www.geometrictools.com/LibMathematics/Containment/Wm5ContSphere3.cpp
/*   With annotations by PRW

template <typename Real>
Sphere3<Real> MergeSpheres (const Sphere3<Real>& sphere0,
    const Sphere3<Real>& sphere1)
{
      // dist sqr between sphere centers
    Vector3<Real> cenDiff = sphere1.Center - sphere0.Center;
    Real lenSqr = cenDiff.SquaredLength();
    
      // dist sqr of sphere radii
    Real rDiff = sphere1.Radius - sphere0.Radius;
    Real rDiffSqr = rDiff*rDiff;

      // If radii are bigger than distance, then they overlap...
      // but I think the assumption that that means that one or the other
      // spheres completely encompasses the other is bogus.
    if (rDiffSqr >= lenSqr)
    {
        return (rDiff >= (Real)0 ? sphere1 : sphere0);
    }

    Real length = Math<Real>::Sqrt(lenSqr);
    Sphere3<Real> sphere;

      // If the distance between spheres is greater than epsilon
    if (length > Math<Real>::ZERO_TOLERANCE)
    {
          // make the sphere center something averaged between the distance
          // and the radius "distance" offset from sphere0
        Real coeff = (length + rDiff)/(((Real)2)*length);
        sphere.Center = sphere0.Center + coeff*cenDiff;
    }
    else
    {
          // sphere0 encompasses sphere1, so just use its center
        sphere.Center = sphere0.Center;
    }

      // Calc new sphere radius as half of the dist between centers plus
      // the two radii
      // This is no good for
    sphere.Radius = ((Real)0.5)*(length + sphere0.Radius + sphere1.Radius);

    return sphere;
}

From Ogre: http://www.ogre3d.org/docs/api/1.9/OgreSphere_8h_source.html

         void merge(const Sphere& oth)
  101         {
  102             Vector3 diff = oth.getCenter() - mCenter;
  103             Real lengthSq = diff.squaredLength();
  104             Real radiusDiff = oth.getRadius() - mRadius;
  105             
  106             // Early-out
  107             if (Math::Sqr(radiusDiff) >= lengthSq) 
  108             {
  109                 // One fully contains the other
  110                 if (radiusDiff <= 0.0f) 
  111                     return; // no change
  112                 else 
  113                 {
  114                     mCenter = oth.getCenter();
  115                     mRadius = oth.getRadius();
  116                     return;
  117                 }
  118             }
  119 
  120             Real length = Math::Sqrt(lengthSq);
  121             Real t = (length + radiusDiff) / (2.0f * length);
  122             mCenter = mCenter + diff * t;
  123             mRadius = 0.5f * (length + mRadius + oth.getRadius());
  124         }
  125


*/

  // So... we do a somewhat different thing than the algorithms above...
  // doesn't seem to be wrong (unit tests, analytically) and is potentially
  // less cases and less computation.  Really don't get how these two
  // implementations are so similar, yet not objectively good.  Hmmm...
  // does make me wonder about this algo, but moving on for now. PRW

void
sphere::
extendBy ( const sphere& sphere )
{
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
