/////////////////////////////////////////////////////////////////////
//
//	file: cylinder.C
//
/////////////////////////////////////////////////////////////////////

#include "pnicylinder.h"
#include "pnivec3.h"
#include "pnicontains.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

cylinder::
cylinder () :
	radius ( -TraitType::oneVal ), 
	halfLength ( TraitType::zeroVal )
{

}

cylinder::
cylinder ( const vec3& centerIn, ValueType radiusIn, const vec3& axisIn, ValueType halfLengthIn ) :
	center ( centerIn ),
	radius ( radiusIn ),
	axis ( axisIn ),
	halfLength ( halfLengthIn )
{

}

cylinder::
cylinder ( InitState ) :
	center ( vec3::NoInit ), 
	axis ( vec3::NoInit )
{

}

cylinder::
cylinder ( const cylinder& orig ) :
	center ( orig.center ), 
	radius ( orig.radius ), 
	axis ( orig.axis ), 
	halfLength ( orig.halfLength )
{

}

cylinder::
~cylinder ()
{

}

/////////////////////////////////////////////////////////////////////

cylinder&
cylinder::
operator = ( const cylinder& orig )
{
	center = orig.center;
	radius = orig.radius;
	axis = orig.axis;
	halfLength = orig.halfLength;

	return *this;
}

/////////////////////////////////////////////////////////////////////
// set methods
void
cylinder::
set ( const vec3& centerIn, ValueType radiusIn, const vec3& axisIn, ValueType halfLengthIn )
{
	center = centerIn;
	radius = radiusIn;
	axis = axisIn;
	halfLength = halfLengthIn;
}

void
cylinder::
setCenter ( const vec3& centerIn )
{
	center = centerIn;
}

void
cylinder::
setRadius ( ValueType radiusIn )
{
	radius = radiusIn;
}

void
cylinder::
setAxis ( const vec3& axisIn )
{
	axis = axisIn;
}

void
cylinder::
setHalfLength ( ValueType halfLengthIn )
{
	halfLength = halfLengthIn;
}

/////////////////////////////////////////////////////////////////////
// get methods

void
cylinder::
get ( vec3& centerOut, ValueType& radiusOut, vec3& axisOut, ValueType& halfLengthOut ) const
{
	centerOut = center;
	radiusOut = radius;
	axisOut = axis;
	halfLengthOut = halfLength;
}


void
cylinder::
getCenter ( vec3& centerIn ) const
{
	centerIn = center;
}

cylinder::ValueType
cylinder::
getRadius () const
{
	return radius;
}

void
cylinder::
getAxis ( vec3& axisIn ) const
{
	axisIn = axis;
}

cylinder::ValueType
cylinder::
getHalfLength () const
{
	return halfLength;
}

/////////////////////////////////////////////////////////////////////
// equality methods
bool
cylinder::
operator == ( const ThisType& other ) const
{
	if ( other.center == center && 
		 other.radius == radius &&
		 other.axis   == axis &&
		 other.halfLength == halfLength )
		return true;
	else
		return false;
}

bool
cylinder::
equal ( const ThisType& other, ValueType fuzz ) const
{
	if ( other.center.equal ( center, fuzz ) &&
		 TraitType::equal ( other.radius, radius, fuzz ) &&
		 other.axis.equal ( axis, fuzz ) &&
		 TraitType::equal ( other.halfLength, halfLength, fuzz ) )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////
// empty methods

void
cylinder::
setEmpty ()
{
	radius = -TraitType::oneVal;
}

bool
cylinder::
isEmpty () const
{
	if ( radius < TraitType::zeroVal )
		return true;
	else
		return false;
}


/////////////////////////////////////////////////////////////////////
// contains methods

int
cylinder::
contains ( ValueType xval, ValueType yval, ValueType zval  ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}


int
cylinder::
contains ( const seg& segIn ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}



int
cylinder::
contains ( const vec3& vec ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}

int
cylinder::
contains ( const sphere& sphere ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}

int
cylinder::
contains ( const box3& box ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}

int
cylinder::
contains ( const cylinder& cyl ) const
{
	PNIMATHUNIMPLEMENTED;
	
	return containsResult::NoneIn;
}


/////////////////////////////////////////////////////////////////////
// extend by methods

void
cylinder::
extendBy ( ValueType xval, ValueType yval, ValueType zval )
{
	PNIMATHUNIMPLEMENTED;
}

void
cylinder::
extendBy ( const vec3& pt )
{
	PNIMATHUNIMPLEMENTED;
}

void
cylinder::
extendBy ( const sphere& sphere )
{
	PNIMATHUNIMPLEMENTED;
}

void
cylinder::
extendBy ( const box3& box )
{
	PNIMATHUNIMPLEMENTED;
}

void
cylinder::
extendBy ( const cylinder& cyl )
{
	PNIMATHUNIMPLEMENTED;
}


/////////////////////////////////////////////////////////////////////
// xform method

void
cylinder::
xformOrtho ( const cylinder& cyl, const matrix4& mat )
{
	PNIMATHUNIMPLEMENTED;
}

void
cylinder::
xformOrtho4 ( const cylinder& cyl, const matrix4& mat )
{
	PNIMATHUNIMPLEMENTED;
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni




