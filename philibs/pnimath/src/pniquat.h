/////////////////////////////////////////////////////////////////////
//
//	class: quat
//
/////////////////////////////////////////////////////////////////////


#ifndef pniquat_h
#define pniquat_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec4.h"
#include "pnivec3.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////
//
// VERY IMPORTANT:
// The imaginary part of quat is stored first.
// The scalar part is in the last element.

class PNIMATHAPI quat :
	public vec4
{ 
	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef quat ThisType;

		quat ();
		quat ( InitState );
		quat ( ValueType x, ValueType y, ValueType z, ValueType w );
		quat ( const ValueType val[ 4 ] );
		quat ( const quat& quatIn );
		~quat ();
		
		// IMPORTANT!!! these methods hide vec4 methods IMPORTANT!!!
		void invert ();
		void setIdentity ();
		bool isIdentity () const;		// exact, not a fuzz compare

		// IMPORTANT!!! these methods hide vec4 methods IMPORTANT!!!
		// length method...?
		ValueType length () const;		

		// IMPORTANT!!! these methods hide vec4 methods IMPORTANT!!!
        // operators
		quat operator * ( const quat &m );		// POST MULT!!!
		quat& operator *= ( const quat &m );		// POST MULT!!!
		quat operator / ( const quat &v );		// INV POST MULT!!!
		quat& operator /= ( const quat &v );		// INV POST MULT!!!
		quat operator- (const quat& m) const;
		quat operator- () const;

		// added quat interface
		// angle-axis methods
		void setRot ( ValueType angle, ValueType xval, ValueType yval, ValueType zval );
		void setRot ( ValueType angle, const vec3& );
		bool getRot ( ValueType& angle, ValueType& xval, ValueType& yval, ValueType& zval ) const;
		bool getRot ( ValueType& angle, vec3& ) const;

		// quat/euler angle conversions.
		void setEuler ( const vec3& hpr );
		void setEuler ( ValueType heading, ValueType pitch, ValueType roll );
		void getEuler ( vec3& hpr ) const;
		void getEuler ( ValueType& heading, ValueType& pitch, ValueType& roll ) const;

		// monadic operations
		ValueType norm () const;
		void conj ();
		void exp ();
		void log ();
		
		// other operations
		// note: lerp and slerp normalize their result and expect
		// normalized arguments!!!
		void lerp ( ValueType t, const quat &q1, const quat &q2 );
		void slerp ( ValueType t, const quat &q1, const quat &q2 );
		void squad ( ValueType t, const quat &q1, const quat &q2, 
				const quat &a, const quat &b );
		void meanTangent ( const quat &q1, const quat &q2, const quat &q3 );
        void mult ( const quat& q1, const quat& q2 );

	protected:
	
	private:
	
};

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
quat::
quat () :
	vec4 ( TraitType::zeroVal, 
			TraitType::zeroVal, 
			TraitType::zeroVal, 
			TraitType::oneVal )
{

}

PNIMATHINLINE
quat::
quat ( InitState initVal ) :
	vec4 ( initVal )
{

}

PNIMATHINLINE
quat::
quat ( ValueType x, ValueType y, ValueType z, ValueType w ) :
	vec4
			( x, y, z, w )
{

}

PNIMATHINLINE
quat::
quat ( const ValueType val[ 4 ] ) :
	vec4 ( val[ 0 ], val[ 1 ], val[ 2 ], val[ 3 ] )
{

}

PNIMATHINLINE
quat::
quat ( const quat& quatIn ) :
	vec4 ( quatIn )
{

}

PNIMATHINLINE
quat::
~quat ()
{

}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
quat::ValueType
quat::
length () const
{
	return vec[ 0 ] * vec[ 0 ] +
		   vec[ 1 ] * vec[ 1 ] +
		   vec[ 2 ] * vec[ 2 ] +
		   vec[ 3 ] * vec[ 3 ];
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
void
quat::
setIdentity ()
{
	vec[ 0 ] = TraitType::zeroVal;
	vec[ 1 ] = TraitType::zeroVal;
	vec[ 2 ] = TraitType::zeroVal;
	vec[ 3 ] = TraitType::oneVal;
}

PNIMATHINLINE
bool
quat::
isIdentity () const
{
	if ( vec[ 0 ] == TraitType::zeroVal &&
			vec[ 1 ] == TraitType::zeroVal &&
			vec[ 2 ] == TraitType::zeroVal &&
			vec[ 3 ] == TraitType::oneVal )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////
// angle-axis method inlines

PNIMATHINLINE
void
quat::
setRot ( ValueType angle, const vec3& vecIn )
{
	setRot ( angle, vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}

PNIMATHINLINE
bool
quat::
getRot ( ValueType& angle, vec3& vecIn ) const
{
	return getRot ( angle, vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
quat::ValueType
quat::
norm () const
{
	return ( vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3] );		
}

PNIMATHINLINE
void
quat::
conj ()
{
	vec[ 0 ] = -vec[ 0 ];
	vec[ 1 ] = -vec[ 1 ];
	vec[ 2 ] = -vec[ 2 ];
//	vec[ 3 ] = vec[ 3 ];	// no-op
}

PNIMATHINLINE
void
quat::
invert ()
{
	conj ();
//	this->vec4::operator /= ( norm () );	// norm is always one for unit quat
}



/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
quat
quat::
operator * ( const quat &in )
{
	quat tmpq ( NoInit );
	
	tmpq.mult ( in, *this ); 
	//tmpq.mult ( *this ,in);

	return tmpq;
}



PNIMATHINLINE
quat quat::operator- (const quat& rhs) const
{
    return quat(vec[0]-rhs.vec[0], vec[1]-rhs.vec[1], 
		vec[2]-rhs.vec[2], vec[3]-rhs.vec[3]);
}

PNIMATHINLINE
quat quat::operator- () const
{
    return quat(-vec[0],-vec[1],-vec[2],-vec[3]);
}

PNIMATHINLINE
quat&
quat::
operator *= ( const quat &in )
{
	mult ( in, *this );
	return *this;
}

PNIMATHINLINE
quat
quat::
operator / ( const quat &in )
{
	quat tmp ( in );
	tmp.invert ();
	
	return *this * tmp;
}

PNIMATHINLINE
quat&
quat::
operator /= ( const quat &in )
{
	quat tmp ( in );
	tmp.invert ();

	*this *= tmp;

	return *this;
}


PNIMATHINLINE
void
quat::
setEuler ( const vec3& hpr )
{
	setEuler ( hpr[0], hpr[1], hpr[2] );
}


PNIMATHINLINE
void
quat::
getEuler ( vec3& hpr ) const
{
	getEuler ( hpr[0], hpr[1], hpr[2] );
}


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pniquat_h
