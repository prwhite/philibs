/////////////////////////////////////////////////////////////////////
//
//	class: plane
//
//	This stores the plane equation as:
//		normal[ 0 ]x + normal[ 1 ]y + normal[ 2 ]z - offset = 0
//	Note: offset is negated!!!
//
/////////////////////////////////////////////////////////////////////


#ifndef pniplane_h
#define pniplane_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec3.h"
#include "pnicontains.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class sphere;
class cylinder;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI plane 
{
	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef plane ThisType;

		enum InitState { NoInit };		
	
		plane ();
		plane ( InitState );
		plane ( const vec3& norm, ValueType offsetIn );
		plane ( ValueType xval, ValueType yval, ValueType zval, 
				ValueType offsetIn );
		
		// like setPts
		plane ( const vec3& pt1, const vec3& pt2, const vec3& pt3 );
		// like setNormPt
		plane ( const vec3& norm, const vec3& ptOnplane );
		
		plane ( const plane& orig );
		~plane ();
		
		// set methods
		plane& operator = ( const plane& orig );
		void set ( const vec3& normIn, ValueType offsetIn );
		void set ( ValueType xval, ValueType yval, ValueType zval, 
				ValueType offsetIn );

		// get methods
		void get ( vec3& normOut, ValueType& offsetOut ) const;
		void getNorm ( vec3& normOut ) const;
		ValueType getOffset () const;

		// equality methods
		bool operator == ( const ThisType& vecIn ) const;
		bool equal ( const ThisType& vecIn, ValueType fuzz = TraitType::fuzzVal ) const;

		// emptiness methods
		void setEmpty ();
		bool isEmpty () const;

		// set a plane from points or a point and a normal
		void setPts ( const vec3& pt1, const vec3& pt2, const vec3& pt3 );
		void setNormPt ( const vec3& norm, const vec3& ptOnplane );

		// simple displacement method
		// move the plane along its normal (positive values move in the
		// normal's direction or in the second variant move the plane to
		// the given point but maintain the current normal
		void displace ( ValueType howFar );
		void displace ( const vec3& pt );
		
		// get the closest point to pt in the primitive
		// args in different order than pfplanes!!!
		bool closestPtIn ( vec3& dest, 
				ValueType xval, ValueType yval, ValueType zval ) const;
		bool closestPtIn ( vec3& dest, const vec3& pt ) const;
		
		// distance from the plane to a point
		ValueType distToPt ( ValueType xval, ValueType yval, ValueType zval ) const;
		ValueType distToPt ( const vec3& pt ) const;
		
		// distance to a point from the plane
		// this is signed, so if the point is in the direction
		// of the normal it will be positive
		ValueType distToPtSigned ( ValueType xval, ValueType yval, ValueType zval ) const;
		ValueType distToPtSigned ( const vec3& pt ) const;
		
      /// Mirror point across this plane
      /// @note Not a reflection 'bouncing' off the plane.
    void mirror ( vec3& dest, vec3 const& src );
  
		// extendBy methods
		void extendBy ( const vec3& pt );
		void extendBy ( const box3& boxIn );
		void extendBy ( const sphere& sphereIn );
		void extendBy ( const seg& segIn );

		// contains methods
		// these methods check for containment of the object in it's 
		// half space on the side the normal points
		int contains ( ValueType xval, ValueType yval, ValueType zval ) const;
		int contains ( const vec3& pt ) const;
		int contains ( const box3& box ) const;
		int contains ( const sphere& sphere ) const;
		int contains ( const cylinder& cyl ) const;
		int contains ( const seg& segIn ) const;
		int contains ( const plane& planeIn ) const;
		
		// xform methods
		void xformOrtho ( const plane& planeIn, const matrix4& mat );  
		void xformOrtho4 ( const plane& planeIn, const matrix4& mat );  

	protected:
		vec3 normal;
		ValueType offset;


	private:

};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
plane::
plane () :
	offset ( TraitType::zeroVal )
{
	
}

PNIMATHINLINE
plane::
plane ( InitState ) :
	normal ( vec3::NoInit )
{
	
}

PNIMATHINLINE
plane::
plane ( const vec3& norm, ValueType offsetIn ) :
	normal ( norm ), 
	offset ( offsetIn )
{
	
}

PNIMATHINLINE
plane::
plane ( ValueType xval, ValueType yval, ValueType zval, ValueType offsetIn ) :
	normal ( xval, yval, zval ), 
	offset ( offsetIn )
{
	
}


PNIMATHINLINE
plane::
plane ( const plane& orig ) :
	normal ( orig.normal ), 
	offset ( orig.offset )
{
	
}

PNIMATHINLINE
plane::
~plane ()
{
	
}

/////////////////////////////////////////////////////////////////////
// set methods
PNIMATHINLINE
void
plane::
set ( const vec3& normIn, ValueType offsetIn )
{
	normal = normIn;
	offset = offsetIn;
}

PNIMATHINLINE
void
plane::
set ( ValueType xval, ValueType yval, ValueType zval, ValueType offsetIn )
{
	normal.set ( xval, yval, zval );
	offset = offsetIn;
}

/////////////////////////////////////////////////////////////////////
// get methods
PNIMATHINLINE
void
plane::
get ( vec3& normOut, ValueType& offsetOut ) const
{
	normOut = normal;
	offsetOut = offset;
}

PNIMATHINLINE
void
plane::
getNorm ( vec3& normOut ) const
{
	normOut = normal;
}

PNIMATHINLINE
plane::ValueType
plane::
getOffset () const
{
	return offset;
}

/////////////////////////////////////////////////////////////////////
// emptiness methods
PNIMATHINLINE
void
plane::
setEmpty ()
{
	normal.set ( TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal );
}

/////////////////////////////////////////////////////////////////////
// distance from the plane to a point
PNIMATHINLINE
plane::ValueType
plane::
distToPt ( ValueType xval, ValueType yval, ValueType zval ) const
{
	return TraitType::abs ( distToPtSigned ( xval, yval, zval ) );
}

PNIMATHINLINE
plane::ValueType
plane::
distToPt ( const vec3& pt ) const
{
	return TraitType::abs ( 
			distToPtSigned ( pt.vec[ 0 ], pt.vec[ 1 ], pt.vec[ 2 ] ) );
}

/////////////////////////////////////////////////////////////////////
// distance to a point from the plane
PNIMATHINLINE
plane::ValueType
plane::
distToPtSigned ( const vec3& pt ) const
{
	return distToPtSigned ( pt.vec[ 0 ], pt.vec[ 1 ], pt.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
int
plane::
contains ( const vec3& pt ) const
{
	return contains ( pt.vec[ 0 ], pt.vec[ 1 ], pt.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pniplane_h




