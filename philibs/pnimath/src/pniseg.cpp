/////////////////////////////////////////////////////////////////////
//
//	file: seg.C
//
/////////////////////////////////////////////////////////////////////

#include "pniseg.h"
#include "pnivec2.h"
#include "pnimatrix4.h"
#include "pnicontains.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////
// set methods

void
seg::
set ( const vec3& posIn, const vec3& endIn )
{
	PNIMATHUNTESTED;

	// these two lines are equivalent to dir = endIn - pos;
	// but don't create a temporary
	dir = endIn;
	pos = posIn;
	dir -= pos;
	
	length = dir.length ();
	dir.normalize ();
}

void
seg::
set ( const vec3& posIn, ValueType headingIn, ValueType pitchIn, 
		ValueType lengthIn )
{
	PNIMATHUNTESTED;

	matrix4 mat ( matrix4::NoInit );
	mat.setEuler ( headingIn, pitchIn, 0.0f );
	dir.set ( 0.0f, TraitType::oneVal, 0.0f );

	dir.xformVec ( dir, mat );
	length = lengthIn;
	pos = posIn;
}

/////////////////////////////////////////////////////////////////////
// equality methods
bool
seg::
operator == ( const ThisType& other ) const
{
	if ( other.pos == pos && 
		 other.dir == dir &&
		 other.length == length )
		return true;
	else
		return false;
}

bool
seg::
equal ( const ThisType& other, ValueType fuzz ) const
{
	if ( other.pos.equal ( pos, fuzz ) && 
		 other.dir.equal ( dir, fuzz ) &&
		 TraitType::equal ( other.length, length, fuzz ) )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////


// OPTIMIZE: NON-OPTIMAL
int
seg::
contains ( const vec3& pt ) const
{
	// TODO: There are some expensive calls in here.  Find a more optimal means of doing this.
	if ( pt.equal ( pos ) )
		return containsResult::AllIn;

	vec3 endPt ( dir );
	endPt *= length;
	endPt += pos;

	if ( pt.equal ( endPt ) )
		return containsResult::AllIn;

//	vec3 ptDir = pt - pos;
	vec3 ptDir ( pt );
	ptDir -= pos;
	vec3 a = ptDir;
	a.normalize ();

	// Direction vectors the same?
	if ( ! a.equal ( dir ) )
	{
		// Nope.  We're not in/on the segment.
		return containsResult::NoneIn;
	}

	// The direction vectors are the same, so what is our distance from the endpoint?
	if ( ptDir.length () <= length )
	{
		return containsResult::AllIn;
	}

	return containsResult::NoneIn;
}



int
seg::
contains ( ValueType x, ValueType y, ValueType z ) const
{
	return contains ( vec3 ( x, y, z ) );
}




// OPTIMIZE: NON-OPTIMAL

int
seg::
contains ( const seg& segIn ) const
{
	// TODO: Remove direction vector normalization??  User's responsibility???
	vec3 tDir ( segIn.dir );
	tDir.normalize ();
//	vec3 endPt = segIn.pos + tDir * segIn.length;
	vec3 endPt ( tDir );
	endPt *= segIn.length;
	endPt += segIn.pos;

	int c1 = contains ( segIn.pos );
	int c2 = contains ( endPt );

	if ( c1 )
	{
		if ( c2 )
			return containsResult::AllIn;

		return containsResult::SomeIn;
	}

	if ( c2 )
		return containsResult::SomeIn;


	// OPTIMIZE: Expensive...
	if ( segIn.contains ( pos ) )
		return containsResult::SomeIn;

	vec3 endPos = pos + dir * length;

	if ( segIn.contains ( endPos ) )
		return containsResult::SomeIn;

	return containsResult::NoneIn;
}








// extendBy ()

bool
seg::
extendBy ( const vec3& pt )
{
	// TODO: This only handles cases where the point is along the "positive" direction
	// of travel.  If it were along the "negative", we should move pos and extend length.
//	vec3 ptDir = pt - pos;
	vec3 ptDir ( pt );
	ptDir -= pos;
	ptDir.normalize ();
	vec3 tDir ( dir );
	tDir.normalize ();

	if ( ! tDir.equal ( ptDir ) )
	{
		return false;
	}

	length = pt.dist ( pos );
	return true;
}

// References:
//   http://geomalgorithms.com/a05-_intersect-1.html

// PNIMATHUNTESTED

bool seg::isect ( ThisType const& rhs, vec3& dst ) const
{
  Trait::ValueType dot = dir.dot(rhs.dir);
  
    // Check if parallel... we say false if they are parallel, even if they
    // are coincident... which isn't exactly true, but for practical
    // purposes it's fine.
    // TODO: Do something sensible with overlapping segments with the same dir.
  if ( Trait::equal(dot, 1.0f) || Trait::equal(dot,-1.0f) )
    return false;

    // Do next step in 2D, as the projection of a 3D line to 2D will still
    // cross at a given parametric value... which can then be extended back
    // into 3D.
    // WARNING: It's possible that projecting into 2D space loses key
    // info (e.g., the 3D lines are in yz plane, so throwing away z info
    // causes the solution to be overlapping lines, rather than a single
    // intersection point.  :(
  vec2 u2 { -dir[ 1 ], dir[ 0 ] };  // right-handed perpendicular
  vec2 v2 = { rhs.dir[ 0 ], rhs.dir[ 1 ] };
  vec2 w2 = { pos[ 0 ] - rhs.pos[ 0 ], pos[ 1 ] - rhs.pos[ 1 ] };
  Trait::ValueType denom = u2.dot ( v2 );
  
    // Calc tu, the distance the isect happens on rhs seg
    // TODO: We don't need tv... just test tu, and then lerp
    // with that.
  Trait::ValueType tv = u2.dot ( w2 ) / denom;
  
    // Isect doesn't happen on rhs segment
  if ( tv < Trait::zeroVal || tv > rhs.length )
    return false;
  
    // Calc tv, the distance the isect happens on this seg
    // website says this should be -vperp, but vperp tests out correctly,
    // so maybe our sense of negative is out of whack.
    // TODO: verify analytically
  v2.set ( -rhs.dir[ 1 ], rhs.dir[ 0 ] ); // rh perp, * -1.0
  
  Trait::ValueType tu = v2.dot(w2) / denom;
  
    // Isect doesn't happen on this segment
  if ( tu < Trait::zeroVal || tu > length )
    return false;
  
  lerp ( dst, tu / length );
  
  return true;
}



bool
seg::
closestPtIn ( vec3& dest, ValueType xval, ValueType yval, ValueType zval ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return false;
}

bool
seg::
closestPtIn ( vec3& dest, const vec3& pt ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return false;
}

/////////////////////////////////////////////////////////////////////
// simple interp method

vec3
seg::
lerp ( ValueType value ) const
{
	vec3 tmp;
	lerp ( tmp, value );
	return tmp;
}

void
seg::
lerp ( vec3& dest, ValueType value ) const
{
	PNIMATHUNTESTED;
	
	dest = dir;
	
	// multiply the normalized direction vector by it's length modified
	// by the t-value passed in
	dest *= value * length;
	
	// add to the base to get the real-world end position
	dest += pos;
}

/////////////////////////////////////////////////////////////////////
// xform methods

void
seg::
xform ( const seg& seg, const matrix4& mat )
{
	PNIMATHUNTESTED;

	// get endpoint of seg
	vec3 end ( math::vec3::NoInit );
	seg.lerp ( end, TraitType::oneVal );
	
	// xform seg's beginning and ending
	end.xformPt ( end, mat );
	pos.xformPt ( seg.pos, mat );
	
	// finish making this seg
	// the following code is equivalent to: set ( pos, end );
	// but it doesn't redundantly set pos again
	dir = end;
	dir -= pos;
	
	length = dir.length ();

	dir /= length;	
}

void
seg::
xform4 ( const seg& seg, const matrix4& mat )
{
	PNIMATHUNTESTED;

	// get endpoint of seg
	vec3 end ( math::vec3::NoInit );
	seg.lerp ( end, TraitType::oneVal );
	
	// xform seg's beginning and ending
	end.xformPt4 ( end, mat );
	pos.xformPt4 ( seg.pos, mat );
	
	// finish making this seg
	// the following code is equivalent to: set ( pos, end );
	// but it doesn't redundantly set pos again
	dir = end;
	dir -= pos;
	
	length = dir.length ();

	dir /= length;	
}



/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

