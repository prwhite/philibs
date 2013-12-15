/////////////////////////////////////////////////////////////////////
//
//	file: pnifrustum.cpp
//
/////////////////////////////////////////////////////////////////////

#include "pnifrustum.h"
#include "pnivec3.h"
#include "pniplane.h"
#include "pnisphere.h"
#include "pnicylinder.h"
#include "pnibox3.h"
#include "pnicontains.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

frustum::
frustum () :
	type ( Ortho )
{

}

frustum::
frustum ( InitState ) :
	type ( Perspective )
{
}

frustum::
frustum ( ValueType nearIn, ValueType farIn,
		  ValueType leftIn, ValueType rightIn,
		  ValueType bottomIn, ValueType topIn ) :
	nearDistance ( nearIn ),
	farDistance ( farIn ),
	left ( leftIn ),
	right ( rightIn ),
	bottom ( bottomIn ),
	top ( topIn ),
	type ( Perspective )
{
	recompute ();
}

frustum::
frustum ( const frustum& orig )
{
	*this = orig;
}



frustum::
~frustum ()
{
}

frustum&
frustum::
operator = ( const frustum& orig )
{
	for ( int k = 0;  k < 6;  k++ )
	{
		planes[ k ] = orig.planes [ k ];
	}

	nearDistance = orig.nearDistance;
	farDistance = orig.farDistance;
	left = orig.left;
	right = orig.right;
	top = orig.top;
	bottom = orig.bottom;
	type = orig.type;
	//xformMatrix = orig.xformMatrix;

	return *this;
}

void
frustum::
set ( ValueType nearIn, ValueType farIn,
	  ValueType leftIn, ValueType rightIn,
	  ValueType bottomIn, ValueType topIn )
{
	nearDistance = nearIn;
	farDistance = farIn;
	left = leftIn;
	right = rightIn;
	top = topIn;
	bottom = bottomIn;

	recompute ();
}

void
frustum::
get ( ValueType& nearOut, ValueType& farOut,
	  ValueType& leftOut, ValueType& rightOut,
	  ValueType& bottomOut, ValueType& topOut ) const
{
	nearOut = nearDistance;
	farOut = farDistance;
	leftOut = left;
	rightOut = right;
	topOut = top;
	bottomOut = bottom;
}

// sets and gets
frustum::ValueType
frustum::
getNearDistance () const
{
	return nearDistance;
}

void
frustum::
setNearDistance ( ValueType nearIn )
{
	nearDistance = nearIn;
	recompute ();
}

frustum::ValueType
frustum::
getFarDistance () const
{
	return farDistance;
}

void
frustum::
setFarDistance ( ValueType farIn )
{
	farDistance = farIn;
	recompute ();
}

// Left/right/top/bottom planes.
frustum::ValueType
frustum::
getLeft () const
{
	return left;
}

void
frustum::
setLeft ( ValueType valIn )
{
	left = valIn;
	recompute();
}

frustum::ValueType
frustum::
getRight () const
{
	return right;
}

void
frustum::
setRight ( ValueType valIn )
{
	right = valIn;
	recompute();
}

frustum::ValueType
frustum::
getTop () const
{
	return top;
}

void
frustum::
setTop ( ValueType valIn )
{
	top = valIn;
	recompute();
}

frustum::ValueType
frustum::
getBottom () const
{
	return bottom;
}

void
frustum::
setBottom ( ValueType valIn )
{
	bottom = valIn;
	recompute();
}

// Extract plane information.

void 
frustum::
getPlane ( plane& dst, PlaneIndex which )
{
	dst = planes[ which ];
}

void
frustum::
getExtents ( vec3 points[ 8 ] )
{
	points[ NearBottomLeft ].set ( left, nearDistance, bottom );
	points[ NearBottomRight ].set ( right, nearDistance, bottom );

	points[ NearTopLeft ].set ( left, nearDistance, top );
	points[ NearTopRight ].set ( right, nearDistance, top );

	if ( type == Ortho )
	{
		points[ FarBottomLeft ].set ( left, farDistance, bottom );
		points[ FarBottomRight ].set ( right, farDistance, bottom );

		points[ FarTopLeft ].set ( left, farDistance, top );
		points[ FarTopRight ].set ( right, farDistance, top );
	}
	else
	{
		// Use similar triangles to figure out left and right
		// at far plane for both symmetric and asymmetric
		// frustums.
		float ratio = farDistance / nearDistance;

		float farLeft = left * ratio;
		float farRight = right * ratio;

		float farBottom = bottom * ratio;
		float farTop = top * ratio;

		points[ FarBottomLeft ].set ( farLeft, farDistance, farBottom );
		points[ FarBottomRight ].set ( farRight, farDistance, farBottom );

		points[ FarTopLeft ].set ( farLeft, farDistance, farTop );
		points[ FarTopRight ].set ( farRight, farDistance, farTop );
	}
}

// Xform interface.
#if 0
void
frustum::
getMatrix ( matrix4& matrixOut ) const
{
	matrixOut = xformMatrix;
}
#endif

void
frustum::
xformOrtho ( const frustum& src, const matrix4& mat )
{
	for ( int k = 0;  k < 6;  k++ )
	{
		planes[ k ].xformOrtho ( planes[ k ], mat );
	}
}

void
frustum::
xformOrtho4 ( const frustum& src, const matrix4& mat )
{
	for ( int k = 0;  k < 6;  k++ )
	{
		planes[ k ].xformOrtho4 ( planes[ k ], mat );
	}
}

// extendBy methods
void
frustum::
extendBy ( const vec3& pt )
{
	for ( int k = 0;  k < 6;  k++ )
	{
		planes[k].extendBy ( pt );
	}

	// TODO: Fix up the frustum values and the resultant transformation matrix.
}

void
frustum::
extendBy ( const box3& boxIn )
{
	for ( int k = 0;  k < 6;  k++ )
	{
		planes[k].extendBy ( boxIn );
	}

	// TODO: Fix up the frustum values and the resultant transformation matrix.
}

void
frustum::
extendBy ( const sphere& sphereIn )
{
	for ( int k = 0;  k < 6;  k++ )
	{
		planes[k].extendBy ( sphereIn );
	}

	// TODO: Fix up the frustum values and the resultant transformation matrix.
}

void
frustum::
extendBy ( const seg& segIn )
{
	for ( int k = 0;  k < 6;  k++ )
	{
		planes[k].extendBy ( segIn );
	}

	// TODO: Fix up the frustum values and the resultant transformation matrix.
}

// Containment of various types.
int
frustum::
contains ( const vec3& pt ) const
{
	int contains = containsResult::SomeIn | containsResult::AllIn;

	for ( int k = 0;  k < 6;  k++ )
	{
		int c = planes[ k ].contains ( pt );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
	}

	return contains;
}

int
frustum::
contains ( const sphere& sphere ) const
{
	int contains = containsResult::SomeIn | containsResult::AllIn;

	for ( int k = 0;  k < 6;  k++ )
	{
		int c = planes[ k ].contains ( sphere );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
	}

	return contains;
}

int
frustum::
contains ( const box3& box ) const
{
	int contains = containsResult::SomeIn | containsResult::AllIn;

	for ( int k = 0;  k < 6;  k++ )
	{
		int c = planes[ k ].contains ( box );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
	}

	return contains;
}


int
frustum::
contains ( const seg& segIn ) const
{
	int contains = containsResult::SomeIn | containsResult::AllIn;

	for ( int k = 0;  k < 6;  k++ )
	{
		int c = planes[ k ].contains ( segIn );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
	}

	return contains;
}

int
frustum::
contains ( const frustum& frustIn ) const
{
	int contains = containsResult::SomeIn | containsResult::AllIn;

	if ( this == &frustIn )
		return contains;

	for ( int k = 0;  k < 6;  k++ )
	{
		for ( int l = 0;  l < 6;  l++ )
		{
			int c = planes[ k ].contains ( frustIn.planes[ l ] );

			if ( c == containsResult::NoneIn )
			{
				return containsResult::NoneIn;
			}

			contains &= c;
		}
	}

	return contains;
}

int
frustum::
contains ( const cylinder& cyl ) const
{
	// TODO: pni::plane doesn't implement the cylinder contains() method.
	// Once pni:plane implements contains () for a cylinder, this should work...
	PNIMATHUNIMPLEMENTED;

	int contains = containsResult::SomeIn | containsResult::AllIn;

	for ( int k = 0;  k < 6;  k++ )
	{
		int c = planes[ k ].contains ( cyl );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
	}

	return contains;
}

void
frustum::
recompute ()
{
	if ( type == Ortho )
	{
		planes[ FarPlane ].set  ( TraitType::zeroVal, -TraitType::oneVal, TraitType::zeroVal, -farDistance );
		planes[ NearPlane ].set ( TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal, nearDistance );

		planes[ LeftPlane ].set  ( TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal, left );
		planes[ RightPlane ].set ( -TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal, -right );

		planes[ BottomPlane ].set ( TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal, bottom );
		planes[ TopPlane ].set    ( TraitType::zeroVal, TraitType::zeroVal, -TraitType::oneVal, -top );
	}
	else
	{
		// Set up the planes.
		planes[ FarPlane ].set  ( TraitType::zeroVal, -TraitType::oneVal, TraitType::zeroVal, -farDistance );
		planes[ NearPlane ].set ( TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal, nearDistance );

		// this approach uses the law of similar triangles to 
		// avoid trig functions... although it uses a divide.
		vec3 norm ( vec3::NoInit );

		norm.set ( -nearDistance, right, TraitType::zeroVal );
		norm.normalize ();
		planes[ RightPlane ].set ( norm, TraitType::zeroVal );

		norm.set ( nearDistance, -left, TraitType::zeroVal );
		norm.normalize ();
		planes[ LeftPlane ].set ( norm, TraitType::zeroVal );

		norm.set ( TraitType::zeroVal, top, -nearDistance );
		norm.normalize ();
		planes[ TopPlane ].set ( norm, TraitType::zeroVal );

		norm.set ( TraitType::zeroVal, -bottom, nearDistance );
		norm.normalize ();
		planes[ BottomPlane ].set ( norm, TraitType::zeroVal );
	}
}



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end of namespace pni 


