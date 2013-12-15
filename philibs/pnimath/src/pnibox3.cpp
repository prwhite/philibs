/////////////////////////////////////////////////////////////////////
//
//	box3.C
//
/////////////////////////////////////////////////////////////////////

#include "pnibox3.h"
#include "pnivec4.h"
#include "pnisphere.h"
#include "pniseg.h"

#include <cmath>
#include <vector>


/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////
// equality methods
bool
box3::
operator == ( const ThisType& other ) const
{
	if ( other.minPos == minPos && other.maxPos == maxPos )
		return true;
	else
		return false;
}

bool
box3::
equal ( const ThisType& other, ValueType fuzz ) const
{
	if ( other.minPos.equal ( minPos, fuzz ) && other.maxPos.equal ( maxPos, fuzz ) )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////
// inlined utility methods

template< class ValueType > 
inline
static
ValueType
box3Max ( ValueType a, ValueType b )
{
	return ( a > b ? a : b );
}

template< class ValueType >
inline
static
ValueType
box3Min ( ValueType a, ValueType b )
{
	return ( a < b ? a : b );
}

/////////////////////////////////////////////////////////////////////

void
box3::
extendBy ( ValueType xval, ValueType yval, ValueType zval )
{
	minPos[ 0 ] = box3Min ( minPos.vec[ 0 ], xval );
	minPos[ 1 ] = box3Min ( minPos.vec[ 1 ], yval );
	minPos[ 2 ] = box3Min ( minPos.vec[ 2 ], zval );

	maxPos[ 0 ] = box3Max ( maxPos.vec[ 0 ], xval );
	maxPos[ 1 ] = box3Max ( maxPos.vec[ 1 ], yval );
	maxPos[ 2 ] = box3Max ( maxPos.vec[ 2 ], zval );
}

void
box3::
extendBy ( const sphere& sphereIn )
{
	if ( !sphereIn.isEmpty() )
	{
		extendBy ( sphereIn.center[ 0 ] + sphereIn.radius, 
					sphereIn.center[ 1 ] + sphereIn.radius, 
					sphereIn.center[ 2 ] + sphereIn.radius );
		extendBy ( sphereIn.center[ 0 ] - sphereIn.radius, 
					sphereIn.center[ 1 ] - sphereIn.radius, 
					sphereIn.center[ 2 ] - sphereIn.radius );
	}
}



void
box3::
extendBy ( const seg& segIn )
{
	vec3 segPos ( math::vec3::NoInit );
	vec3 v2 ( math::vec3::NoInit );
	ValueType segLen;
	segIn.get ( segPos, v2, segLen );
//	v2.normalize ();
//	v2 = segPos + v2 * segLen;
	v2 *= segLen;
	v2 += segPos;

	extendBy ( segPos );
	extendBy ( v2 );
}

/////////////////////////////////////////////////////////////////////

bool
box3::
closestPtIn ( vec3& dest, ValueType xval, ValueType yval, ValueType zval ) const
{
	if ( isEmpty () )
		return false;

	if ( xval < minPos[ 0 ] )
		dest.vec[ 0 ] = minPos[ 0 ];
	else if ( xval > maxPos[ 0 ] )
		dest.vec[ 0 ] = maxPos[ 0 ];
	else
		dest.vec[ 0 ] = xval;

	if ( yval < minPos[ 1 ] )
		dest.vec[ 1 ] = minPos[ 1 ];
	else if ( yval > maxPos[ 1 ] )
		dest.vec[ 1 ] = maxPos[ 1 ];
	else
		dest.vec[ 1 ] = yval;

	if ( zval < minPos[ 2 ] )
		dest.vec[ 2 ] = minPos[ 2 ];
	else if ( zval > maxPos[ 2 ] )
		dest.vec[ 2 ] = maxPos[ 2 ];
	else
		dest.vec[ 2 ] = zval;

	return true;
}


/////////////////////////////////////////////////////////////////////

int
box3::
contains ( const box3& in ) const
{
		// do trivial rejects first
	if (in.minPos[ 0 ] > maxPos[ 0 ] || in.minPos[ 1 ] > maxPos[ 1 ]
		|| in.minPos[ 2 ] > maxPos[ 2 ])
		return containsResult::NoneIn;

	if (in.maxPos[ 0 ] < minPos[ 0 ] || in.maxPos[ 1 ] < minPos[ 1 ]
		|| in.maxPos[ 2 ] < minPos[ 2 ])
		return containsResult::NoneIn;

		// check if completely in
	if (contains( in.minPos ) && contains( in.maxPos ))
		return (containsResult::AllIn | containsResult::SomeIn);

		// only possibility left
	return( containsResult::SomeIn );
}

/*

// What the f was R*lph thinking?  This is a mess and 
// completely inefficient.  See the replacement isect 
// method below for a lesson in simplicity.

#ifdef THISISTHEBIGGESTPIECEOFCRAPEVER

// Anonymous namespace
namespace {

class segBoxIsect
{
	public:
		segBoxIsect ()  {}
		virtual ~segBoxIsect ()  {}

		int intersect ( const box3& boxIn, const seg& segIn );
		//int intersect ( const box3& box, const vec3& segPos, const vec3& segDir, const vec3& segEndPos );
		int getNumPoints ();
		vec3 getPoint ( unsigned int idx );


	private:
		typedef std::vector<vec3> pointList;   // By value.

		pointList isectPoints;
};



// TODO: OPTIMIZE: This is long and gruesome code.
// There has to be a more elegant and computationally efficient means of doing this.

int
segBoxIsect::
intersect ( const box3& boxIn, const seg& segIn )
{
	// Following code snippet was adapated from Graphics Gems I, pp. 736-737
	// Unfortunately, this algorithm "culls" backfaces.
	// Should find a better/faster algorithm.

	enum Side { LeftSide, Middle, RightSide };
	const int NumDimen = 3;

	int sides[ 2 ][ NumDimen ];
	Trait::ValueType planePos[ 2 ][ NumDimen ];
	bool ptInside[ 2 ];
	ptInside[ 0 ] = true;
	ptInside[ 1 ] = true;

	// Clear out any previous intersect points.
	isectPoints.erase ( isectPoints.begin (), isectPoints.end () );

	// Get all of the points in question.
	vec3 segPos[ 2 ];
	vec3 segDir[ 2 ];
	Trait::ValueType len;
	segIn.get ( segPos[ 0 ], segDir[ 0 ], len );
	segPos[ 1 ] = segPos[ 0 ] + segDir[ 0 ] * len;
	segDir[ 1 ] = segDir[ 0 ] * -1.0;

	// Get the box stuff.
	vec3 minPos;
	vec3 maxPos;
	boxIn.get ( minPos, maxPos );


	int j;
	int k;

	for ( j = 0;  j < 2;  j++ )
	{
		for ( k = 0;  k < NumDimen;  k++ )
		{
			if ( segPos[ j ][ k ] < minPos[ k ] )
			{
				ptInside[ j ] = false;   // The segment pos is NOT inside the box.
				sides[ j ][ k ] = LeftSide;
				planePos[ j ][ k ] = minPos[ k ];
			}
			else if ( segPos[ j ][ k ] > maxPos[ k ] )
			{
				ptInside[ j ] = false;   // The segment pos is NOT inside the box.
				sides[ j ][ k ] = RightSide;
				planePos[ j ][ k ] = maxPos[ k ];
			}
			else
			{
				sides[ j ][ k ] = Middle;
			}
		}    // for ( k ...
	}    // for ( j ...

	if ( ptInside[ 0 ] && ptInside[ 1 ] )
	{
		// Both points are on the interior.
		return 0;
	}

	// parametric value.
	Trait::ValueType maxT[ 2 ][ NumDimen ];

	for ( j = 0;  j < 2;  j++ )
	{
		if ( ptInside[ j ] )
			continue;

		for ( k = 0;  k < NumDimen;  k++ )
		{
			if ( sides[ j ][ k ] != Middle  &&  segDir[ j ][ k ] != 0.0 )
			{
				maxT[ j ][ k ] = ( planePos[ j ][ k ] - segPos[ j ][ k ] ) / segDir[ j ][ k ];
			}
			else
			{
				maxT[ j ][ k ] = -1.0;
			}
		}

		int whichPlane = 0;
		for ( k = 0;  k < NumDimen;  k++ )
			if ( maxT[ j ][ whichPlane ] < maxT[ j ][ k ] )
				whichPlane = k;

		// Check if there is an intersection or not.
		if ( maxT[ j ][ whichPlane ] < 0.0 )
			continue;   // Nope.  No intersect.

		bool addPoint = true;
		vec3 tVec ( vec3::NoInit );

		for ( k = 0;  k < NumDimen;  k++ )
		{

			if ( whichPlane != k )
			{
				tVec[ k ] = segPos[ j ][ k ] + maxT[ j ][ whichPlane ] * segDir[ j ][ k ];
				
				if ( ( sides[ j ][ k ] == RightSide  &&  segPos[ j ][ k ] < minPos[ k ] ) ||
					 ( sides[ j ][ k ] == LeftSide  &&  segPos[ j ][ k ] > maxPos[ k ] ) )
					 {
					 addPoint = false;
					 break;
					 }
			}
			else
				tVec[ k ] = planePos[ j ][ k ];
		}

		if ( addPoint && segIn.contains ( tVec ) )
		{
			isectPoints.push_back ( tVec );
		}
	}

	return isectPoints.size ();
}




int
segBoxIsect::
getNumPoints ()
{
	return isectPoints.size ();
}




vec3
segBoxIsect::
getPoint ( unsigned int idx )
{
	if ( idx > isectPoints.size () )
	{
		// Throw an exception here?
		//throw;
		return vec3 ( vec3::NoInit );
	}

	return isectPoints[ idx ];
}




}    // end of anonymous namespace

#endif // THISISTHEBIGGESTPIECEOFCRAPEVER
*/

int
box3::
contains ( const seg& segIn ) const
{
	if ( isEmpty () )
		return containsResult::NoneIn;

	vec3 pos ( math::vec3::NoInit );
	vec3 dir ( math::vec3::NoInit );
	ValueType len;
	segIn.get ( pos, dir, len );
//	dir.normalize ();
//	vec3 endPos = pos + dir * len;
	vec3 endPos ( dir );
	endPos *= len;
	endPos += pos;

	// The segment begin and endpoint must lie in the box.
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

/*
#ifdef THISISTHEBIGGESTPIECEOFCRAPEVER
	// Use utility class above.
	segBoxIsect sbIsect;

	if ( sbIsect.intersect ( *this, segIn ) > 0 )
	{
		return containsResult::SomeIn;
	}
#endif // THISISTHEBIGGESTPIECEOFCRAPEVER
*/

	float tnear, tfar;
	if ( isect ( segIn, tnear, tfar ) )
		return containsResult::SomeIn;

	return containsResult::NoneIn;
}




// NON-OPTIMAL

int
box3::
contains ( const sphere& in ) const
{
	vec3 center ( math::vec3::NoInit );
	ValueType radius;
	in.get ( center, radius );

	if ( ( center[ 0 ] - minPos[ 0 ] ) >= radius  &&
		 ( center[ 1 ] - minPos[ 1 ] ) >= radius  &&
		 ( center[ 2 ] - minPos[ 2 ] ) >= radius  &&
		 ( maxPos[ 0 ] - center[ 0 ] ) >= radius  &&
		 ( maxPos[ 1 ] - center[ 1 ] ) >= radius  &&
		 ( maxPos[ 2 ] - center[ 2 ] ) >= radius )
		 return containsResult::AllIn | containsResult::SomeIn;

	if ( contains ( center ) )
		return containsResult::SomeIn;

	vec3 p1 = minPos - center;
	p1.normalize ();
//	p1 = center + p1 * radius;
	p1 *= radius;
	p1 += center;

	if ( contains ( p1 ) )
		return containsResult::SomeIn;

	vec3 p2 = minPos - center;
	p2.normalize ();
//	p2 = center + p2 * radius;
	p2 *= radius;
	p2 += center;

	if ( contains ( p2 ) )
		return containsResult::SomeIn;

	return containsResult::NoneIn;
}


int
box3::
contains ( const cylinder& cylIn ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}

template< class Type >
inline
void
testAndMaybeSwap ( Type& lhs, Type& rhs )
{
	if ( lhs > rhs )
	{
		Type tmp = lhs;
		lhs = rhs;
		rhs = tmp;
	}
}

///////
// From http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm .

// This modifies tnearVal and tfar val even if there is
// no intersection!!!

bool 
box3::
isect ( const math::seg& segIn, ValueType& tnear, ValueType& tfar ) const
{
	tnear = -Trait::maxVal;
	tfar = Trait::maxVal;

	for ( int num = 0; num < 3; ++num )
	{
		ValueType orig = segIn.getPos ()[ num ];
		ValueType dir = segIn.getDir ()[ num ];

		if ( ! Trait::equal ( dir, Trait::zeroVal ) )
		{
			ValueType anear = ( minPos[ num ] - orig ) / dir;
			ValueType afar = ( maxPos[ num ] - orig ) / dir;
			
			testAndMaybeSwap ( anear, afar );
			tnear = anear > tnear ? anear : tnear;
			tfar = afar < tfar ? afar : tfar;
		}
		else
		{
			if ( orig <= minPos[ num ] || orig >= maxPos[ num ] )
				return false;
		}
	}

	// Note... the "=" is very important for things that are flat in
	// any dimension.  Of course, this might be better as math::Trait::equal.
	return ( tnear <= tfar );
}


/////////////////////////////////////////////////////////////////////

bool
box3::
getCorner ( vec3& dst, Corners xaxis, Corners yaxis, Corners zaxis ) const
{
#ifdef PNIMATHDEBUG
	if ( isEmpty () )
		return false;
#endif

	if ( xaxis == Min )
		dst.vec[ 0 ] = minPos.vec[ 0 ];
	else
		dst.vec[ 0 ] = maxPos.vec[ 0 ];

	if ( yaxis == Min )
		dst.vec[ 1 ] = minPos.vec[ 1 ];
	else
		dst.vec[ 1 ] = maxPos.vec[ 1 ];

	if ( zaxis == Min )
		dst.vec[ 2 ] = minPos.vec[ 2 ];
	else
		dst.vec[ 2 ] = maxPos.vec[ 2 ];
	
	return true;
}

/////////////////////////////////////////////////////////////////////
// xform methods

void
box3::
xform ( const box3& box, const matrix4& mat )
{
	if ( box.isEmpty () )
	{
		setEmpty();
		return;
	}

	vec3 tmin ( box.minPos );
	vec3 tmax ( box.maxPos );
	
	setEmpty ();
	
	vec3 cur;
	
	cur.set ( tmin.vec[ 0 ], tmin.vec[ 1 ], tmin.vec[ 2 ] );
	cur.xformPt ( cur, mat );
	extendBy ( cur );

	cur.set ( tmax.vec[ 0 ], tmin.vec[ 1 ], tmin.vec[ 2 ] );
	cur.xformPt ( cur, mat );
	extendBy ( cur );

	cur.set ( tmin.vec[ 0 ], tmax.vec[ 1 ], tmin.vec[ 2 ] );
	cur.xformPt ( cur, mat );
	extendBy ( cur );

	cur.set ( tmin.vec[ 0 ], tmin.vec[ 1 ], tmax.vec[ 2 ] );
	cur.xformPt ( cur, mat );
	extendBy ( cur );


	cur.set ( tmax.vec[ 0 ], tmax.vec[ 1 ], tmax.vec[ 2 ] );
	cur.xformPt ( cur, mat );
	extendBy ( cur );

	cur.set ( tmin.vec[ 0 ], tmax.vec[ 1 ], tmax.vec[ 2 ] );
	cur.xformPt ( cur, mat );
	extendBy ( cur );

	cur.set ( tmax.vec[ 0 ], tmin.vec[ 1 ], tmax.vec[ 2 ] );
	cur.xformPt ( cur, mat );
	extendBy ( cur );

	cur.set ( tmax.vec[ 0 ], tmax.vec[ 1 ], tmin.vec[ 2 ] );
	cur.xformPt ( cur, mat );
	extendBy ( cur );
}

inline static
void
conv ( vec3& dst, const vec4& src )
{
	dst[ 0 ] = src[ 0 ];
	dst[ 1 ] = src[ 1 ];
	dst[ 2 ] = src[ 2 ];
	
	dst /= src[ 3 ];
}

#ifdef BOXCONV3TO4NEEDED
inline static
void
conv ( vec4& dst, const vec3& src )
{
	dst[ 0 ] = src[ 0 ];
	dst[ 1 ] = src[ 1 ];
	dst[ 2 ] = src[ 2 ];
	dst[ 3 ] = 1.0f;
}
#endif

// this method could use the xformPt4 method of vec3, but
// we have unwound that method here to save on needless
// temporaries and function calls

void
box3::
xform4 ( const box3& box, const matrix4& mat )
{
	if ( isEmpty () )
		return;

	vec3 tmin ( box.minPos );
	vec3 tmax ( box.maxPos );
	vec3 vec3 ( vec3::NoInit );
	vec4 cur ( vec4::NoInit );
	
	setEmpty ();
	
	cur.set ( tmin.vec[ 0 ], tmin.vec[ 1 ], tmin.vec[ 2 ], 1.0f );
	cur.xformPt ( cur, mat );
	conv ( vec3, cur );
	extendBy ( vec3 );

	cur.set ( tmax.vec[ 0 ], tmin.vec[ 1 ], tmin.vec[ 2 ], 1.0f );
	cur.xformPt ( cur, mat );
	conv ( vec3, cur );
	extendBy ( vec3 );

	cur.set ( tmin.vec[ 0 ], tmax.vec[ 1 ], tmin.vec[ 2 ], 1.0f );
	cur.xformPt ( cur, mat );
	conv ( vec3, cur );
	extendBy ( vec3 );

	cur.set ( tmin.vec[ 0 ], tmin.vec[ 1 ], tmax.vec[ 2 ], 1.0f );
	cur.xformPt ( cur, mat );
	conv ( vec3, cur );
	extendBy ( vec3 );


	cur.set ( tmax.vec[ 0 ], tmax.vec[ 1 ], tmax.vec[ 2 ], 1.0f );
	cur.xformPt ( cur, mat );
	conv ( vec3, cur );
	extendBy ( vec3 );

	cur.set ( tmin.vec[ 0 ], tmax.vec[ 1 ], tmax.vec[ 2 ], 1.0f );
	cur.xformPt ( cur, mat );
	conv ( vec3, cur );
	extendBy ( vec3 );

	cur.set ( tmax.vec[ 0 ], tmin.vec[ 1 ], tmax.vec[ 2 ], 1.0f );
	cur.xformPt ( cur, mat );
	conv ( vec3, cur );
	extendBy ( vec3 );

	cur.set ( tmax.vec[ 0 ], tmax.vec[ 1 ], tmin.vec[ 2 ], 1.0f );
	cur.xformPt ( cur, mat );
	conv ( vec3, cur );
	extendBy ( vec3 );
}



/////////////////////////////////////////////////////////////////////


	} // end namespace math
} // end namespace pni
