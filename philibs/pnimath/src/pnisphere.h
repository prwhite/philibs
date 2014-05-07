/////////////////////////////////////////////////////////////////////
//
//	class: sphere
//
/////////////////////////////////////////////////////////////////////


#ifndef pnisphere_h
#define pnisphere_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec3.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class box3;
class cylinder;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI sphere 
{
	friend class box3;
	friend class plane;

	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef sphere ThisType;

		// Enumeration moved out to pnicontains.h
		//enum Contains { NoneIn = 0, SomeIn = 1, AllIn = 2 };
		enum InitState { NoInit };

		sphere ();
		sphere ( InitState );
		sphere ( const vec3& centerIn, ValueType radiusIn );
		sphere ( ValueType xval, ValueType yval, ValueType zval, ValueType radiusIn );
		sphere ( const sphere& orig );
		
		~sphere ();
		
		// set methods
		sphere& operator = ( const sphere& orig );
		void set ( const vec3& centerIn, ValueType radiusIn );
		void set ( ValueType xval, ValueType yval, ValueType zval, ValueType radiusIn );

		// get methods
		void get ( vec3& centerOut, ValueType& radiusOut ) const;
		void getCenter ( vec3& centerOut ) const;
    vec3 const& getCenter () const { return center; }
		ValueType getRadius () const;

		// equality methods
		bool operator == ( const ThisType& vecIn ) const;
		bool equal ( const ThisType& vecIn, ValueType fuzz = TraitType::fuzzVal ) const;

		// empty methods
		void setEmpty ();
		bool  isEmpty () const;

		// contains methods
		int contains ( ValueType xval, ValueType yval, ValueType zval ) const;
		int contains ( const vec3& pt ) const;
		int contains ( const sphere& sphere ) const;
		int contains ( const box3& box ) const;
		int contains ( const cylinder& cyl ) const;
		int contains ( const seg& segIn ) const;
						
		// get the closest point to pt in the primative
		bool closestPtIn ( vec3& dest, 
				ValueType xval, ValueType yval, ValueType zval ) const;
		bool closestPtIn ( vec3& dest, const vec3& pt ) const;
 
 		// extend by methods
		void extendBy ( ValueType xval, ValueType yval, ValueType zval );
		void extendBy ( const vec3& pt );
		void extendBy ( const sphere& sphere );
		void extendBy ( const box3& box );
		void extendBy ( const cylinder& cyl );

		// xform method
		void xformOrtho ( const sphere& sphere, const matrix4& mat );
		void xformOrtho4 ( const sphere& sphere, const matrix4& mat );

	protected:
		vec3 center;
		ValueType radius;
		

	private:

};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// empty methods
// this method comes before the constructor that calls it because
// of inlining

PNIMATHINLINE
void
sphere::
setEmpty ()
{
	radius = -TraitType::oneVal;		// any negative value is bogus
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
sphere::
sphere ()
{
	setEmpty ();
}

PNIMATHINLINE
sphere::
sphere ( InitState ) :
	center ( vec3::NoInit )
{
	
}

PNIMATHINLINE
sphere::
sphere ( const vec3& centerIn, ValueType radiusIn ) :
	center ( centerIn ), 
	radius ( radiusIn )
{
	
}

PNIMATHINLINE
sphere::
sphere ( ValueType xval, ValueType yval, ValueType zval, ValueType radiusIn ) :
	center ( xval, yval, zval ), 
	radius ( radiusIn )
{
	
}

PNIMATHINLINE
sphere::
sphere ( const sphere& orig ) :
	center ( orig.center ), 
	radius ( orig.radius )
{

}

PNIMATHINLINE
sphere::
~sphere ()
{

}

/////////////////////////////////////////////////////////////////////
// set methods

PNIMATHINLINE
sphere&
sphere::
operator = ( const sphere& orig )
{
	center = orig.center;
	radius = orig.radius;
	
	return *this;
}

PNIMATHINLINE
void
sphere::
set ( const vec3& centerIn, ValueType radiusIn )
{
	center = centerIn;
	radius = radiusIn;
}

PNIMATHINLINE
void
sphere::
set ( ValueType xval, ValueType yval, ValueType zval, ValueType radiusIn )
{
	center.set ( xval, yval, zval );
	radius = radiusIn;
}

/////////////////////////////////////////////////////////////////////
// get methods

PNIMATHINLINE
void
sphere::
get ( vec3& centerOut, ValueType& radiusOut ) const
{
	centerOut = center;
	radiusOut = radius;
}

PNIMATHINLINE
void
sphere::
getCenter ( vec3& centerOut ) const
{
	centerOut = center;
}

PNIMATHINLINE
sphere::ValueType
sphere::
getRadius () const
{
	return radius;
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
int
sphere::
contains ( const vec3& pt ) const
{
	return contains ( pt.vec[ 0 ], pt.vec[ 1 ], pt.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
bool
sphere::
closestPtIn ( vec3& dest, const vec3& pt ) const
{
	return closestPtIn ( dest, pt.vec[ 0 ], pt.vec[ 1 ], pt.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnisphere_h
