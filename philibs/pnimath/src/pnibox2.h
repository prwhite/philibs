/////////////////////////////////////////////////////////////////////
//
//	class: box2
//
/////////////////////////////////////////////////////////////////////


#ifndef pnibox2_h
#define pnibox2_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec2.h"
#include "pnicontains.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

		class matrix4;
//		class sphere;
//		class cylinder;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI box2 { 
	//friend class sphere;

	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef box2 ThisType;

		enum InitState { NoInit };
	
		box2 ();
		box2 ( InitState );
		box2 ( ValueType minx, ValueType miny, 
				ValueType maxx, ValueType maxy );
		box2 ( const vec2& minPos, 
				const vec2& maxPox );
		box2 ( const box2& orig );
		~box2 ();
		
		// set methods
		box2& operator = ( const box2& orig );
		void set ( const vec2& minIn, const vec2& maxIn );
		void set ( ValueType minx, ValueType miny, 
				   ValueType maxx, ValueType maxy );

		// get methods
		void get ( vec2& minOut, vec2& maxOut ) const;
		void getMin ( vec2& minOut ) const;
		void getMax ( vec2& maxOut ) const;
		
		// equality methods
		bool operator == ( const ThisType& vecIn ) const;
		bool equal ( const ThisType& vecIn, ValueType fuzz = TraitType::fuzzVal ) const;

		// emptiness methods
		void setEmpty ();
		bool isEmpty () const;
		
		// extend box methods
		void extendBy ( ValueType xval, ValueType yval );
		void extendBy ( const vec2& vecIn );
		void extendBy ( const box2& boxIn );

			//these two may want to implemented for circle, 2D seg
		//void extendBy ( const sphere& sphereIn );
		//void extendBy ( const seg& segIn );

		//void extendBy ( const cylinder& cylIn );
		//void extendBy ( const frustum& frustIn );
		//void extendBy ( const polytope& ptopeIn );

		// intersect methods
		void intersect( const box2& boxIn );
		
		// get the closest point to pt in the primative
		bool closestPtIn ( vec2& dest, 
				ValueType xval, ValueType yval ) const;
		bool closestPtIn ( vec2& dest, const vec2& pt ) const;
 
 		// contain methods
		int contains ( ValueType xval, ValueType yval ) const;
		int contains ( const vec2& vecIn  ) const;
		int contains ( const box2& boxIn  ) const;

			//may want to reimplement 2D stuff
//		int contains ( const sphere& sphereIn  ) const;
//		int contains ( const cylinder& cylIn  ) const;
//		int contains ( const seg& segIn ) const;

		//int contains ( const frustum& frustIn ) const;
		//int contains ( const polytope& ptopeIn ) const;
		
		// center methods
		bool getCenter ( ValueType& xval, ValueType& yval ) const;
		bool getCenter ( vec2& dst ) const;
		
		// corner methods these are experimental
		// getCorner ( dst, Min, Min, Min ) -> dst == min
		// getCorner ( dst, Max, Max, Max ) -> dst == max
		enum Corners { Min, Max };
		bool getCorner ( vec2& dst, Corners xaxis, Corners yaxis  ) const;
		
		// xform method
		// may want 2D versions
//		void xform ( const box2& box, const matrix4& mat );
//		void xform4 ( const box2& box, const matrix4& mat );
		
	protected:
		vec2 minPos;
		vec2 maxPos;
		
	private:
	
};

/////////////////////////////////////////////////////////////////////

// setEmpty def must precede any caller of setEmpty

PNIMATHINLINE
void
box2::
setEmpty ()
{
	minPos.set ( TraitType::maxVal, TraitType::maxVal );
	maxPos.set ( -TraitType::maxVal, -TraitType::maxVal );
}


PNIMATHINLINE
box2::
box2 ()
{
	setEmpty ();
}


PNIMATHINLINE
box2::
box2 ( InitState initVal ) :
	minPos ( vec2::NoInit ), 
	maxPos ( vec2::NoInit )
{
	
}


PNIMATHINLINE
box2::
box2 ( ValueType minx, ValueType miny, 
		ValueType maxx, ValueType maxy ) :
	minPos ( minx, miny ), 
	maxPos ( maxx, maxy )
{

}


PNIMATHINLINE
box2::
box2 ( const vec2& minIn, const vec2& maxIn ) :
	minPos ( minIn ), 
	maxPos ( maxIn )
{

}



PNIMATHINLINE
box2::
box2 ( const box2& in ) :
	minPos ( in.minPos ), 
	maxPos ( in.maxPos )
{
	
}

PNIMATHINLINE
box2&
box2::
operator = ( const box2& in )
{
	minPos = in.minPos;
	maxPos = in.maxPos;
	
	return *this;
}

PNIMATHINLINE
void
box2::
set ( const vec2& minIn, const vec2& maxIn )
{
	minPos = minIn;
	maxPos = maxIn;
}

PNIMATHINLINE
void
box2::
set ( ValueType minx, ValueType miny, 
	  ValueType maxx, ValueType maxy  )
{
	minPos.set ( minx, miny );
	maxPos.set ( maxx, maxy );
}


PNIMATHINLINE
box2::
~box2 ()
{
	
}

PNIMATHINLINE
void
box2::
get ( vec2& minOut, vec2& maxOut ) const
{
	minOut = minPos;
	maxOut = maxPos;
}

PNIMATHINLINE
void
box2::
getMin ( vec2& minOut ) const
{
	minOut = minPos;
}

PNIMATHINLINE
void
box2::
getMax ( vec2& maxOut ) const
{
	maxOut = maxPos;
}

/////////////////////////////////////////////////////////////////////


PNIMATHINLINE
bool
box2::
isEmpty () const
{
		//check each dimension
	if ( minPos.vec[ 0 ] >= maxPos.vec[ 0 ] || minPos.vec[ 1 ] >= maxPos.vec[ 1 ] )
		return true;
	else
		return false;
}


/////////////////////////////////////////////////////////////////////



PNIMATHINLINE
void
box2::
extendBy ( const vec2& vecIn )
{
	extendBy ( vecIn.vec[ 0 ], vecIn.vec[ 1 ] );
}


PNIMATHINLINE
void
box2::
extendBy ( const box2& boxIn )
{
	if ( !boxIn.isEmpty() )
	{
		extendBy ( boxIn.minPos.vec[ 0 ], boxIn.minPos.vec[ 1 ] );
		extendBy ( boxIn.maxPos.vec[ 0 ], boxIn.maxPos.vec[ 1 ] );
	}
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
bool
box2::
closestPtIn ( vec2& dest, const vec2& pt ) const
{
	return closestPtIn ( dest, pt.vec[ 0 ], pt.vec[ 1 ] );
}

/////////////////////////////////////////////////////////////////////


PNIMATHINLINE
int
box2::
contains ( ValueType xval, ValueType yval ) const
{
	if ( xval >= minPos.vec[ 0 ] &&
		 yval >= minPos.vec[ 1 ] &&
		 xval <= maxPos.vec[ 0 ] &&
		 yval <= maxPos.vec[ 1 ] )
		return containsResult::AllIn;
	else
		return containsResult::NoneIn;
}


PNIMATHINLINE
int
box2::
contains ( const vec2& vecIn ) const
{
	return contains ( vecIn.vec[ 0 ], vecIn.vec[ 1 ] );
}

PNIMATHINLINE
bool
box2::
getCenter ( ValueType& xval, ValueType& yval ) const
{
#ifdef PNIMATHDEBUG
	if ( isEmpty () )
		return false;
#endif

	xval = ( minPos.vec[ 0 ] + maxPos.vec[ 0 ] ) * ( ValueType ) 0.5;
	yval = ( minPos.vec[ 1 ] + maxPos.vec[ 1 ] ) * ( ValueType ) 0.5;
	
	return true;
}


PNIMATHINLINE
bool
box2::
getCenter ( vec2& dst ) const
{
	return getCenter ( dst.vec[ 0 ], dst.vec[ 1 ] );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnibox2_h
