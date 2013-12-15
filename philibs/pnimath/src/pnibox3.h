/////////////////////////////////////////////////////////////////////
//
//	class: box3
//
/////////////////////////////////////////////////////////////////////


#ifndef pnibox3_h
#define pnibox3_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec3.h"
#include "pnicontains.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

		class matrix4;
		class sphere;
		class cylinder;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI box3 { 
	friend class sphere;

	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef box3 ThisType;

		enum InitState { NoInit };
	
		box3 ();
		box3 ( InitState );
		box3 ( ValueType minx, ValueType miny, ValueType minz, 
				ValueType maxx, ValueType maxy, ValueType maxz );
		box3 ( const vec3& minPos, 
				const vec3& maxPox );
		box3 ( const box3& orig );
		~box3 ();
		
		// set methods
		box3& operator = ( const box3& orig );
		void set ( const vec3& minIn, const vec3& maxIn );
		void set ( ValueType minx, ValueType miny, ValueType minz, 
				   ValueType maxx, ValueType maxy, ValueType maxz );

		void grow ( float howMuch );

		// get methods
		void get ( vec3& minOut, vec3& maxOut ) const;
		void getMin ( vec3& minOut ) const;
		void getMax ( vec3& maxOut ) const;
		void getSize ( vec3& sizeOut ) const;
		
		// equality methods
		bool operator == ( const ThisType& vecIn ) const;
		bool equal ( const ThisType& vecIn, ValueType fuzz = TraitType::fuzzVal ) const;

		// emptiness methods
		void setEmpty ();
		bool isEmpty () const;
		
		// extend box methods
		void extendBy ( ValueType xval, ValueType yval, ValueType zval );
		void extendBy ( const vec3& vecIn );
		void extendBy ( const box3& boxIn );
		void extendBy ( const sphere& sphereIn );
		void extendBy ( const seg& segIn );
		//void extendBy ( const cylinder& cylIn );
		//void extendBy ( const frustum& frustIn );
		//void extendBy ( const polytope& ptopeIn );
		
		// get the closest point to pt in the primative
		bool closestPtIn ( vec3& dest, 
				ValueType xval, ValueType yval, ValueType zval ) const;
		bool closestPtIn ( vec3& dest, const vec3& pt ) const;
 
 		// contain methods
		int contains ( ValueType xval, ValueType yval, ValueType zval ) const;
		int contains ( const vec3& vecIn  ) const;
		int contains ( const box3& boxIn  ) const;
		int contains ( const sphere& sphereIn  ) const;
		int contains ( const cylinder& cylIn  ) const;
		int contains ( const seg& segIn ) const;
		//int contains ( const frustum& frustIn ) const;
		//int contains ( const polytope& ptopeIn ) const;
		
		  // Notes:
		  // Modifies tnear and tfar even for a miss.
		  // A seg starting inside the box will have a negative tnear.
		bool isect ( const seg& segIn, ValueType& tnear, ValueType& tfar ) const;
		
		// center methods
		bool getCenter ( ValueType& xval, ValueType& yval, ValueType& zval ) const;
		bool getCenter ( vec3& dst ) const;
		
		// corner methods these are experimental
		// getCorner ( dst, Min, Min, Min ) -> dst == min
		// getCorner ( dst, Max, Max, Max ) -> dst == max
		enum Corners { Min, Max };
		bool getCorner ( vec3& dst, Corners xaxis, Corners yaxis, Corners zaxis ) const;
		
		// xform method
		void xform ( const box3& box, const matrix4& mat );
		void xform4 ( const box3& box, const matrix4& mat );
		
	protected:
		vec3 minPos;
		vec3 maxPos;
		
	private:
	
};

/////////////////////////////////////////////////////////////////////

// setEmpty def must precede any caller of setEmpty

PNIMATHINLINE
void
box3::
setEmpty ()
{
	minPos.set ( TraitType::maxVal, TraitType::maxVal, TraitType::maxVal );
	maxPos.set ( -TraitType::maxVal, -TraitType::maxVal, -TraitType::maxVal );
}


PNIMATHINLINE
box3::
box3 ()
{
	setEmpty ();
}


PNIMATHINLINE
box3::
box3 ( InitState initVal ) :
	minPos ( vec3::NoInit ), 
	maxPos ( vec3::NoInit )
{
	
}


PNIMATHINLINE
box3::
box3 ( ValueType minx, ValueType miny, ValueType minz, 
		ValueType maxx, ValueType maxy, ValueType maxz ) :
	minPos ( minx, miny, minz ), 
	maxPos ( maxx, maxy, maxz )
{

}


PNIMATHINLINE
box3::
box3 ( const vec3& minIn, const vec3& maxIn ) :
	minPos ( minIn ), 
	maxPos ( maxIn )
{

}



PNIMATHINLINE
box3::
box3 ( const box3& in ) :
	minPos ( in.minPos ), 
	maxPos ( in.maxPos )
{
	
}

PNIMATHINLINE
box3&
box3::
operator = ( const box3& in )
{
	minPos = in.minPos;
	maxPos = in.maxPos;
	
	return *this;
}

PNIMATHINLINE
void
box3::
set ( const vec3& minIn, const vec3& maxIn )
{
	minPos = minIn;
	maxPos = maxIn;
}

PNIMATHINLINE
void
box3::
set ( ValueType minx, ValueType miny, ValueType minz, 
	  ValueType maxx, ValueType maxy, ValueType maxz )
{
	minPos.set ( minx, miny, minz );
	maxPos.set ( maxx, maxy, maxz );
}


PNIMATHINLINE
box3::
~box3 ()
{
	
}

PNIMATHINLINE
void box3::grow ( float howMuch )
{
	minPos -= howMuch;
	maxPos += howMuch;
}

PNIMATHINLINE
void
box3::
get ( vec3& minOut, vec3& maxOut ) const
{
	minOut = minPos;
	maxOut = maxPos;
}

PNIMATHINLINE
void
box3::
getMin ( vec3& minOut ) const
{
	minOut = minPos;
}

PNIMATHINLINE
void
box3::
getMax ( vec3& maxOut ) const
{
	maxOut = maxPos;
}

PNIMATHINLINE
void
box3::
getSize ( vec3& sizeOut ) const
{
	sizeOut = maxPos;
	sizeOut -= minPos;
}

/////////////////////////////////////////////////////////////////////


PNIMATHINLINE
bool
box3::
isEmpty () const
{
	// this test could be exhaustive, but for efficiency the definition
	// of empty is "relaxed"
	if ( minPos.vec[ 0 ] > maxPos.vec[ 0 ] )
		return true;
	else
		return false;
}


/////////////////////////////////////////////////////////////////////



PNIMATHINLINE
void
box3::
extendBy ( const vec3& vecIn )
{
	extendBy ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}


PNIMATHINLINE
void
box3::
extendBy ( const box3& boxIn )
{
	if ( !boxIn.isEmpty() )
	{
		extendBy ( boxIn.minPos.vec[ 0 ], boxIn.minPos.vec[ 1 ], boxIn.minPos.vec[ 2 ] );
		extendBy ( boxIn.maxPos.vec[ 0 ], boxIn.maxPos.vec[ 1 ], boxIn.maxPos.vec[ 2 ] );
	}
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
bool
box3::
closestPtIn ( vec3& dest, const vec3& pt ) const
{
	return closestPtIn ( dest, pt.vec[ 0 ], pt.vec[ 1 ], pt.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////


PNIMATHINLINE
int
box3::
contains ( ValueType xval, ValueType yval, ValueType zval ) const
{
	if ( xval >= minPos.vec[ 0 ] &&
		 yval >= minPos.vec[ 1 ] &&
		 zval >= minPos.vec[ 2 ] &&
		 xval <= maxPos.vec[ 0 ] &&
		 yval <= maxPos.vec[ 1 ] &&
		 zval <= maxPos.vec[ 2 ] )
		return containsResult::SomeIn;
	else
		return containsResult::NoneIn;
}


PNIMATHINLINE
int
box3::
contains ( const vec3& vecIn ) const
{
	return contains ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}

PNIMATHINLINE
bool
box3::
getCenter ( ValueType& xval, ValueType& yval, ValueType& zval ) const
{
#ifdef PNIMATHDEBUG
	if ( isEmpty () )
		return false;
#endif

	xval = ( minPos.vec[ 0 ] + maxPos.vec[ 0 ] ) * ( ValueType ) 0.5;
	yval = ( minPos.vec[ 1 ] + maxPos.vec[ 1 ] ) * ( ValueType ) 0.5;
	zval = ( minPos.vec[ 2 ] + maxPos.vec[ 2 ] ) * ( ValueType ) 0.5;
	
	return true;
}


PNIMATHINLINE
bool
box3::
getCenter ( vec3& dst ) const
{
	return getCenter ( dst.vec[ 0 ], dst.vec[ 1 ], dst.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnibox3_h
