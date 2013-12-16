/////////////////////////////////////////////////////////////////////
//
//	box2.C
//
/////////////////////////////////////////////////////////////////////

#include "pnibox2.h"
#include "pnivec2.h"
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
box2::
operator == ( const ThisType& other ) const
{
	if ( other.minPos == minPos && other.maxPos == maxPos )
		return true;
	else
		return false;
}

bool
box2::
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
box2Max ( ValueType a, ValueType b )
{
	return ( a > b ? a : b );
}

template< class ValueType >
inline
static
ValueType
box2Min ( ValueType a, ValueType b )
{
	return ( a < b ? a : b );
}

/////////////////////////////////////////////////////////////////////

void
box2::
extendBy ( ValueType xval, ValueType yval )
{
	minPos[ 0 ] = box2Min ( minPos.vec[ 0 ], xval );
	minPos[ 1 ] = box2Min ( minPos.vec[ 1 ], yval );

	maxPos[ 0 ] = box2Max ( maxPos.vec[ 0 ], xval );
	maxPos[ 1 ] = box2Max ( maxPos.vec[ 1 ], yval );
}

//void
//box2::
//extendBy ( const sphere& sphereIn )
//{
//	extendBy ( sphereIn.center[ 0 ] + sphereIn.radius, 
//				sphereIn.center[ 1 ] + sphereIn.radius, 
//				sphereIn.center[ 2 ] + sphereIn.radius );
//	extendBy ( sphereIn.center[ 0 ] - sphereIn.radius, 
//				sphereIn.center[ 1 ] - sphereIn.radius, 
//				sphereIn.center[ 2 ] - sphereIn.radius );
//}



//void
//box2::
//extendBy ( const seg& segIn )
//{
//	vec2 segPos;
//	vec2 v2;
//	ValueType segLen;
//	segIn.get ( segPos, v2, segLen );
//	v2.normalize ();
//	v2 = segPos + v2 * segLen;
//
//	extendBy ( segPos );
//	extendBy ( v2 );
//}

void
box2::
intersect( const box2& boxIn )
{
		// check for empty boxes first
		// empty boxes may become valid by below algorithm
	if (isEmpty())
		return;
	if (boxIn.isEmpty())
	{
		setEmpty();
		return;
	}

	minPos[ 0 ] = box2Max( minPos[ 0 ], boxIn.minPos[ 0 ] );
	minPos[ 1 ] = box2Max( minPos[ 1 ], boxIn.minPos[ 1 ] );

	maxPos[ 0 ] = box2Min( maxPos[ 0 ], boxIn.maxPos[ 0 ] );
	maxPos[ 1 ] = box2Min( maxPos[ 1 ], boxIn.maxPos[ 1 ] );
}

/////////////////////////////////////////////////////////////////////

bool
box2::
closestPtIn ( vec2& dest, ValueType xval, ValueType yval ) const
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

	return true;
}


/////////////////////////////////////////////////////////////////////

int
box2::
contains ( const box2& in ) const
{
		// do trivial rejects first
	if (in.minPos[ 0 ] > maxPos[ 0 ] || in.minPos[ 1 ] > maxPos[ 1 ] )
		return containsResult::NoneIn;

	if (in.maxPos[ 0 ] < minPos[ 0 ] || in.maxPos[ 1 ] < minPos[ 1 ] )
		return containsResult::NoneIn;

		// check if completely in
	if (contains( in.minPos ) && contains( in.maxPos ))
		return (containsResult::AllIn | containsResult::SomeIn);

		// only possibility left
	return( containsResult::SomeIn );
}




// Anonymous namespace
namespace {

class segBoxIsect
{
	public:
		segBoxIsect ()  {}
		virtual ~segBoxIsect ()  {}

    //int intersect ( const box2& boxIn, const seg& segIn );
		//int intersect ( const box2& box, const vec2& segPos, const vec2& segDir, const vec2& segEndPos );
		size_t getNumPoints ();
		vec2 getPoint ( unsigned int idx );


	private:
		typedef std::vector<vec2> pointList;   // By value.

		pointList isectPoints;
};





// TODO: OPTIMIZE: This is long and gruesome code.
// There has to be a more elegant and computationally efficient means of doing this.

//int
//segBoxIsect::
//intersect ( const box2& boxIn, const seg& segIn )
//{
//	// Following code snippet was adapated from Graphics Gems I, pp. 736-737
//	// Unfortunately, this algorithm "culls" backfaces.
//	// Should find a better/faster algorithm.
//
//	enum Side { LeftSide, Middle, RightSide };
//	const int NumDimen = 3;
//
//	int sides[ 2 ][ NumDimen ];
//	Trait::ValueType planePos[ 2 ][ NumDimen ];
//	bool ptInside[ 2 ];
//	ptInside[ 0 ] = true;
//	ptInside[ 1 ] = true;
//
//	// Clear out any previous intersect points.
//	isectPoints.erase ( isectPoints.begin (), isectPoints.end () );
//
//	// Get all of the points in question.
//	vec2 segPos[ 2 ];
//	vec2 segDir[ 2 ];
//	Trait::ValueType len;
//	segIn.get ( segPos[ 0 ], segDir[ 0 ], len );
//	segPos[ 1 ] = segPos[ 0 ] + segDir[ 0 ] * len;
//	segDir[ 1 ] = segDir[ 0 ] * -1.0;
//
//	// Get the box stuff.
//	vec2 minPos;
//	vec2 maxPos;
//	boxIn.get ( minPos, maxPos );
//
//
//	int j;
//	int k;
//
//	for ( j = 0;  j < 2;  j++ )
//	{
//		for ( k = 0;  k < NumDimen;  k++ )
//		{
//			if ( segPos[ j ][ k ] < minPos[ k ] )
//			{
//				ptInside[ j ] = false;   // The segment pos is NOT inside the box.
//				sides[ j ][ k ] = LeftSide;
//				planePos[ j ][ k ] = minPos[ k ];
//			}
//			else if ( segPos[ j ][ k ] > maxPos[ k ] )
//			{
//				ptInside[ j ] = false;   // The segment pos is NOT inside the box.
//				sides[ j ][ k ] = RightSide;
//				planePos[ j ][ k ] = maxPos[ k ];
//			}
//			else
//			{
//				sides[ j ][ k ] = Middle;
//			}
//		}    // for ( k ...
//	}    // for ( j ...
//
//	if ( ptInside[ 0 ] && ptInside[ 1 ] )
//	{
//		// Both points are on the interior.
//		return 0;
//	}
//
//	// parametric value.
//	Trait::ValueType maxT[ 2 ][ NumDimen ];
//
//	for ( j = 0;  j < 2;  j++ )
//	{
//		if ( ptInside[ j ] )
//			continue;
//
//		for ( k = 0;  k < NumDimen;  k++ )
//		{
//			if ( sides[ j ][ k ] != Middle  &&  segDir[ j ][ k ] != 0.0 )
//			{
//				maxT[ j ][ k ] = ( planePos[ j ][ k ] - segPos[ j ][ k ] ) / segDir[ j ][ k ];
//			}
//			else
//			{
//				maxT[ j ][ k ] = -1.0;
//			}
//		}
//
//		int whichPlane = 0;
//		for ( k = 0;  k < NumDimen;  k++ )
//			if ( maxT[ j ][ whichPlane ] < maxT[ j ][ k ] )
//				whichPlane = k;
//
//		// Check if there is an intersection or not.
//		if ( maxT[ j ][ whichPlane ] < 0.0 )
//			continue;   // Nope.  No intersect.
//
//		bool addPoint = true;
//		vec2 tVec ( vec2::NoInit );
//
//		for ( k = 0;  k < NumDimen;  k++ )
//		{
//
//			if ( whichPlane != k )
//			{
//				tVec[ k ] = segPos[ j ][ k ] + maxT[ j ][ whichPlane ] * segDir[ j ][ k ];
//				
//				if ( ( sides[ j ][ k ] == RightSide  &&  segPos[ j ][ k ] < minPos[ k ] ) ||
//					 ( sides[ j ][ k ] == LeftSide  &&  segPos[ j ][ k ] > maxPos[ k ] ) )
//					 {
//					 addPoint = false;
//					 break;
//					 }
//			}
//			else
//				tVec[ k ] = planePos[ j ][ k ];
//		}
//
//		if ( addPoint && segIn.contains ( tVec ) )
//		{
//			isectPoints.push_back ( tVec );
//		}
//	}
//
//	return isectPoints.size ();
//}




size_t
segBoxIsect::
getNumPoints ()
{
	return isectPoints.size ();
}




vec2
segBoxIsect::
getPoint ( unsigned int idx )
{
	if ( idx > isectPoints.size () )
	{
		// Throw an exception here?
		//throw;
		return vec2 ( vec2::NoInit );
	}

	return isectPoints[ idx ];
}




}    // end of anonymous namespace




//int
//box2::
//contains ( const seg& segIn ) const
//{
//	vec2 pos;
//	vec2 dir;
//	ValueType len;
//	segIn.get ( pos, dir, len );
//	dir.normalize ();
//	vec2 endPos = pos + dir * len;
//
//	// The segment begin and endpoint must lie in the box.
//	int c1 = contains ( pos );
//	int c2 = contains ( endPos );
//
//	if ( c1 )
//	{
//		if ( c2 )
//			return containsResult::AllIn | containsResult::SomeIn;
//
//		return containsResult::SomeIn;
//	}
//
//	if ( c2 )
//	{
//		return containsResult::SomeIn;
//	}
//
//
//	// Use utility class above.
//	segBoxIsect sbIsect;
//
//	if ( sbIsect.intersect ( *this, segIn ) > 0 )
//	{
//		return containsResult::SomeIn;
//	}
//
//	return containsResult::NoneIn;
//}




// NON-OPTIMAL

//int
//box2::
//contains ( const sphere& in ) const
//{
//	vec2 center;
//	ValueType radius;
//	in.get ( center, radius );
//
//	if ( ( center[ 0 ] - minPos[ 0 ] ) >= radius  &&
//		 ( center[ 1 ] - minPos[ 1 ] ) >= radius  &&
//		 ( center[ 2 ] - minPos[ 2 ] ) >= radius  &&
//		 ( maxPos[ 0 ] - center[ 0 ] ) >= radius  &&
//		 ( maxPos[ 1 ] - center[ 1 ] ) >= radius  &&
//		 ( maxPos[ 2 ] - center[ 2 ] ) >= radius )
//		 return containsResult::AllIn | containsResult::SomeIn;
//
//	if ( contains ( center ) )
//		return containsResult::SomeIn;
//
//	vec2 p1 = minPos - center;
//	p1.normalize ();
//	p1 = center + p1 * radius;
//
//	if ( contains ( p1 ) )
//		return containsResult::SomeIn;
//
//	vec2 p2 = minPos - center;
//	p2.normalize ();
//	p2 = center + p2 * radius;
//
//	if ( contains ( p2 ) )
//		return containsResult::SomeIn;
//
//	return containsResult::NoneIn;
//}



//int
//box2::
//contains ( const cylinder& cylIn ) const
//{
//	PNIMATHUNIMPLEMENTED;
//	
//	return containsResult::NoneIn;
//}



/////////////////////////////////////////////////////////////////////

bool
box2::
getCorner ( vec2& dst, Corners xaxis, Corners yaxis ) const
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
	
	return true;
}

/////////////////////////////////////////////////////////////////////
// xform methods

//void
//box2::
//xform ( const box2& box, const matrix4& mat )
//{
//	vec2 tmin ( box.minPos );
//	vec2 tmax ( box.maxPos );
//	
//	setEmpty ();
//	
//	vec2 cur;
//	
//	cur.set ( tmin.vec[ 0 ], tmin.vec[ 1 ] );
//	cur.xformPt ( cur, mat );
//	extendBy ( cur );
//
//	cur.set ( tmax.vec[ 0 ], tmin.vec[ 1 ] );
//	cur.xformPt ( cur, mat );
//	extendBy ( cur );
//
//	cur.set ( tmin.vec[ 0 ], tmax.vec[ 1 ] );
//	cur.xformPt ( cur, mat );
//	extendBy ( cur );
//
//	cur.set ( tmin.vec[ 0 ], tmin.vec[ 1 ] );
//	cur.xformPt ( cur, mat );
//	extendBy ( cur );
//
//
//	cur.set ( tmax.vec[ 0 ], tmax.vec[ 1 ] );
//	cur.xformPt ( cur, mat );
//	extendBy ( cur );
//
//	cur.set ( tmin.vec[ 0 ], tmax.vec[ 1 ] );
//	cur.xformPt ( cur, mat );
//	extendBy ( cur );
//
//	cur.set ( tmax.vec[ 0 ], tmin.vec[ 1 ] );
//	cur.xformPt ( cur, mat );
//	extendBy ( cur );
//
//	cur.set ( tmax.vec[ 0 ], tmax.vec[ 1 ] );
//	cur.xformPt ( cur, mat );
//	extendBy ( cur );
//}

inline static
void
conv ( vec2& dst, const vec3& src )
{
	dst[ 0 ] = src[ 0 ];
	dst[ 1 ] = src[ 1 ];
	dst[ 2 ] = src[ 2 ];
	
	dst /= src[ 3 ];
}

#ifdef BOXCONV3TO4NEEDED
inline static
void
conv ( vec3& dst, const vec2& src )
{
	dst[ 0 ] = src[ 0 ];
	dst[ 1 ] = src[ 1 ];
	dst[ 2 ] = src[ 2 ];
	dst[ 3 ] = 1.0f;
}
#endif

// this method could use the xformPt4 method of vec2, but
// we have unwound that method here to save on needless
// temporaries and function calls

//void
//box2::
//xform4 ( const box2& box, const matrix4& mat )
//{
//	vec2 tmin ( box.minPos );
//	vec2 tmax ( box.maxPos );
//	vec2 vec2 ( vec2::NoInit );
//	vec3 cur ( vec3::NoInit );
//	
//	setEmpty ();
//	
//	cur.set ( tmin.vec[ 0 ], tmin.vec[ 1 ], tmin.vec[ 2 ], 1.0f );
//	cur.xformPt ( cur, mat );
//	conv ( vec2, cur );
//	extendBy ( vec2 );
//
//	cur.set ( tmax.vec[ 0 ], tmin.vec[ 1 ], tmin.vec[ 2 ], 1.0f );
//	cur.xformPt ( cur, mat );
//	conv ( vec2, cur );
//	extendBy ( vec2 );
//
//	cur.set ( tmin.vec[ 0 ], tmax.vec[ 1 ], tmin.vec[ 2 ], 1.0f );
//	cur.xformPt ( cur, mat );
//	conv ( vec2, cur );
//	extendBy ( vec2 );
//
//	cur.set ( tmin.vec[ 0 ], tmin.vec[ 1 ], tmax.vec[ 2 ], 1.0f );
//	cur.xformPt ( cur, mat );
//	conv ( vec2, cur );
//	extendBy ( vec2 );
//
//
//	cur.set ( tmax.vec[ 0 ], tmax.vec[ 1 ], tmax.vec[ 2 ], 1.0f );
//	cur.xformPt ( cur, mat );
//	conv ( vec2, cur );
//	extendBy ( vec2 );
//
//	cur.set ( tmin.vec[ 0 ], tmax.vec[ 1 ], tmax.vec[ 2 ], 1.0f );
//	cur.xformPt ( cur, mat );
//	conv ( vec2, cur );
//	extendBy ( vec2 );
//
//	cur.set ( tmax.vec[ 0 ], tmin.vec[ 1 ], tmax.vec[ 2 ], 1.0f );
//	cur.xformPt ( cur, mat );
//	conv ( vec2, cur );
//	extendBy ( vec2 );
//
//	cur.set ( tmax.vec[ 0 ], tmax.vec[ 1 ], tmin.vec[ 2 ], 1.0f );
//	cur.xformPt ( cur, mat );
//	conv ( vec2, cur );
//	extendBy ( vec2 );
//}



/////////////////////////////////////////////////////////////////////


	} // end namespace math
} // end namespace pni
