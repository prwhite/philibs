/////////////////////////////////////////////////////////////////////
//
//!	class: matrix4
//
//  A row-major 4x4 matrix class... translation is in the last
//  row ( mat[ 3 ][ 0 - 3 ] ).
//
/////////////////////////////////////////////////////////////////////


#ifndef pnimatrix4_h
#define pnimatrix4_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"

/////////////////////////////////////////////////////////////////////

#include "pnivec3.h"
#include "pniquat.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class vec4;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI matrix4 { 
	public:
		typedef Trait TraitType;
		typedef Trait::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef matrix4 ThisType;

		enum RotAxis { Xaxis, Yaxis, Zaxis };
		enum InitState { NoInit };
		
		// constructors
		matrix4 ();
		matrix4 ( InitState );
		matrix4 ( const ValueType* );
		matrix4 ( const ThisType& matIn );
		matrix4 ( ValueType x0y0, ValueType x0y1, ValueType x0y2, ValueType x0y3, 
					ValueType x1y0, ValueType x1y1, ValueType x1y2, ValueType x1y3, 
					ValueType x2y0, ValueType x2y1, ValueType x2y2, ValueType x2y3,
					ValueType x3y0, ValueType x3y1, ValueType x3y2, ValueType x3y3 );
		~matrix4 ();
		
		void set (  ValueType x0y0, ValueType x0y1, ValueType x0y2, ValueType x0y3, 
					ValueType x1y0, ValueType x1y1, ValueType x1y2, ValueType x1y3, 
					ValueType x2y0, ValueType x2y1, ValueType x2y2, ValueType x2y3,
					ValueType x3y0, ValueType x3y1, ValueType x3y2, ValueType x3y3 );
		void set ( const ValueType* data );
		
		operator ValueType* () { return &mat[ 0 ][ 0 ]; }
		operator const ValueType* () const { return &mat[ 0 ][ 0 ]; }
		
		// all of the add methods are post multiply
		
		void setIdentity ();
		bool isIdentity () const;		// exact, not a fuzz compare
				
		//! translate methods
		void setTrans ( ValueType xval, ValueType yval, ValueType zval );
		void setTrans ( const vec3& );
		void preTrans ( ValueType xval, ValueType yval, ValueType zval );
		void preTrans ( const vec3& );
		void postTrans ( ValueType xval, ValueType yval, ValueType zval );
		void postTrans ( const vec3& );
		bool getTrans ( ValueType& xval, ValueType& yval, ValueType& zval ) const;
		bool getTrans ( vec3& ) const;

		//! angle/axis rotate methods
		void setRot ( ValueType degrees, ValueType, ValueType, ValueType );
		void setRot ( ValueType degrees, RotAxis axis );
		void setRot ( ValueType degrees, const vec3& );

		void preRot ( ValueType degrees, ValueType, ValueType, ValueType );
		void preRot ( ValueType degrees, RotAxis axis );
		void preRot ( ValueType degrees, const vec3& );

		void postRot ( ValueType degrees, ValueType, ValueType, ValueType );
		void postRot ( ValueType degrees, RotAxis axis );
		void postRot ( ValueType degrees, const vec3& );

		bool getRot ( ValueType& degrees, ValueType&, ValueType&, ValueType& ) const;
		bool getRot ( ValueType& degrees, vec3& axis ) const;
		bool getRot ( matrix4& rotMat ) const;
		
		//! quaternion rotation methods
		// for the 4 argument versions, the scalar is the last arg
		void setQuat ( ValueType xval, ValueType yval, ValueType zval, ValueType sval );
		void setQuat ( const quat& quat );
		void preQuat ( ValueType xval, ValueType yval, ValueType zval, ValueType sval );
		void preQuat ( const quat& quat );
		void postQuat ( ValueType xval, ValueType yval, ValueType zval, ValueType sval );
		void postQuat ( const quat& quat );
		bool getQuat ( ValueType& xval, ValueType& yval, ValueType& zval, ValueType& sval ) const;
		bool getQuat ( quat& quat ) const;

		//! misc rotate methods
		void setVecRotVec ( const vec3& v1, const vec3& v2 );

		//! euler rotation methods
		void setEuler ( ValueType, ValueType, ValueType );
		void setEuler ( const vec3& );
		void preEuler ( ValueType hval, ValueType pval, ValueType rval );
		void preEuler ( const vec3& );
		void postEuler ( ValueType hval, ValueType pval, ValueType rval );
		void postEuler ( const vec3& );
		bool getEuler ( ValueType& hval, ValueType& pval, ValueType& rval ) const;
		bool getEuler ( vec3& ) const;

		//! matrix/matrix rot methods... only copies
		// 3x3 sub-matrix... sets rest of matrix to 0's
		// (except for the diagonal)
		void set3x3Mat ( const matrix4& rhs );

		//! coord (trans and euler) methods
		void setCoord ( const vec3& trans, const vec3& eul );
		bool getCoord ( vec3& trans, vec3& eul ) const;
		
		//! scale methods
		void setScale ( ValueType hval, ValueType pval, ValueType rval );
		void setScale ( const vec3& );
		void preScale ( ValueType hval, ValueType pval, ValueType rval );
		void preScale ( const vec3& );
		void postScale ( ValueType hval, ValueType pval, ValueType rval );
		void postScale ( const vec3& );
		bool getScale ( ValueType& hval, ValueType& pval, ValueType& rval ) const;
		bool getScale ( vec3& ) const;

		//! projection methods
		void setOrtho (	ValueType l, ValueType r,
							ValueType b, ValueType t, 
							ValueType n, ValueType f );
		void setFrustum (	ValueType l, ValueType r,
							ValueType b, ValueType t, 
							ValueType n, ValueType f );
		void setPerspective ( ValueType fovy, ValueType aspect, ValueType nearVal, ValueType farVal );
		void setViewport ( ValueType xorig, ValueType yorig, ValueType width, ValueType height );
		
		//! error if from == to or up isn't normlized
		void setLookat ( const vec3& from, const vec3& to, const vec3& up );

		//! get/set rows of the matrix
		void setRow ( int row, ValueType v0, ValueType v1, ValueType v2, ValueType v3 );
		void getRow ( int row, ValueType& v0, ValueType& v1, ValueType& v2, ValueType& v3 ) const;
		void setRow ( int row, ValueType v0, ValueType v1, ValueType v2 );
		void getRow ( int row, ValueType& v0, ValueType& v1, ValueType& v2 ) const;

		void setRow ( int row, const vec4& );
		void getRow ( int row, vec4& ) const;
		void setRow ( int row, const vec3& );
		void getRow ( int row, vec3& ) const;

		//! get/set columns of the matrix
		void setCol ( int row, ValueType v0, ValueType v1, ValueType v2, ValueType v3 );
		void getCol ( int row, ValueType& v0, ValueType& v1, ValueType& v2, ValueType& v3 ) const;
		void setCol ( int row, ValueType v0, ValueType v1, ValueType v2 );
		void getCol ( int row, ValueType& v0, ValueType& v1, ValueType& v2 ) const;

		void setCol ( int col, const vec4& );
		void getCol ( int col, vec4& ) const;
		void setCol ( int col, const vec3& );
		void getCol ( int col, vec3& ) const;
		
		//! misc matrix operations
		void transpose ();
		bool invert ();
		
		//! matrix element accessor methods
		ValueType*       operator [](int i)        { return &mat[i][0]; }
		const ValueType* operator [](int i) const  { return &mat[i][0]; }
		
		//! math operators
		bool operator == ( const ThisType& matIn ) const;
		bool equal ( const ThisType& matIn, ValueType fuzz = TraitType::fuzzVal ) const;
		ThisType operator + ( const ThisType& matIn ) const;
		ThisType operator - ( const ThisType& matIn ) const;
		ThisType operator * ( const ThisType& matIn ) const;	// POST MULT!!!
		ThisType operator / ( const ThisType& matIn ) const;
		ThisType operator * ( ValueType val ) const;
		ThisType operator / ( ValueType val ) const;

		ThisType& operator = ( const ThisType& matIn );
		ThisType& operator += ( const ThisType& matIn );
		ThisType& operator -= ( const ThisType& matIn );
		ThisType& operator *= ( const ThisType& matIn );		// POST MULT!!!
		ThisType& operator /= ( const ThisType& matIn );
		ThisType& operator *= ( ValueType val );
		ThisType& operator /= ( ValueType val );
		
		//! explicit multiplication methods
		void preMult ( const ThisType& mat );
		void postMult ( const ThisType& mat );
		
		//! for multiplying by row/column vectors... the order of
		// args indicates whether the op is pre or post mult.
		// for the vec3 methods, 1 will be used for w
		void rowMult ( matrix4& dmat, const vec4& svec ) const;
		void rowMult ( vec4& dvec, const matrix4& smat ) const;
		void colMult ( const matrix4& smat, vec4& dvec ) const;
		void colMult ( const vec4& svec, matrix4& dmat ) const;

		
	protected:
		friend matrix4::ValueType RCD( const matrix4& A, const matrix4& B, int i, int j);
		
		ValueType mat[ 4 ][ 4 ];
		
	private:
	
};

/////////////////////////////////////////////////////////////////////

// these methods must appear before any callers of these methods
//  when they are in-lineds

PNIMATHINLINE
void
matrix4::
set (	ValueType x0y0, ValueType x0y1, ValueType x0y2, ValueType x0y3, 
		ValueType x1y0, ValueType x1y1, ValueType x1y2, ValueType x1y3, 
		ValueType x2y0, ValueType x2y1, ValueType x2y2, ValueType x2y3,
		ValueType x3y0, ValueType x3y1, ValueType x3y2, ValueType x3y3 )
{
	mat[ 0 ][ 0 ] = x0y0;
	mat[ 0 ][ 1 ] = x0y1;
	mat[ 0 ][ 2 ] = x0y2;
	mat[ 0 ][ 3 ] = x0y3;
	
	mat[ 1 ][ 0 ] = x1y0;
	mat[ 1 ][ 1 ] = x1y1;
	mat[ 1 ][ 2 ] = x1y2;
	mat[ 1 ][ 3 ] = x1y3;
	
	mat[ 2 ][ 0 ] = x2y0;
	mat[ 2 ][ 1 ] = x2y1;
	mat[ 2 ][ 2 ] = x2y2;
	mat[ 2 ][ 3 ] = x2y3;
	
	mat[ 3 ][ 0 ] = x3y0;
	mat[ 3 ][ 1 ] = x3y1;
	mat[ 3 ][ 2 ] = x3y2;
	mat[ 3 ][ 3 ] = x3y3;
} 


PNIMATHINLINE
void
matrix4::
set ( const ValueType* data )
{
	ValueType* start = mat[ 0 ];
	start[ 0 ] = data[ 0 ];
	start[ 1 ] = data[ 1 ];
	start[ 2 ] = data[ 2 ];
	start[ 3 ] = data[ 3 ];
	
	start[ 4 ] = data[ 4 ];
	start[ 5 ] = data[ 5 ];
	start[ 6 ] = data[ 6 ];
	start[ 7 ] = data[ 7 ];
	
	start[ 8 ] = data[ 8 ];
	start[ 9 ] = data[ 9 ];
	start[ 10 ] = data[ 10 ];
	start[ 11 ] = data[ 11 ];
	
	start[ 12 ] = data[ 12 ];
	start[ 13 ] = data[ 13 ];
	start[ 14 ] = data[ 14 ];
	start[ 15 ] = data[ 15 ];
}


PNIMATHINLINE
void
matrix4::
setIdentity ()
{ 
	set ( TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal, 
		  TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal, TraitType::zeroVal, 
		  TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal, TraitType::zeroVal, 
		  TraitType::zeroVal, TraitType::zeroVal, TraitType::zeroVal, TraitType::oneVal );
}

PNIMATHINLINE
bool
matrix4::
isIdentity () const
{
	const ValueType* start = mat[ 0 ];	

	if ( start[ 0 ] == TraitType::oneVal &&
		 start[ 1 ] == TraitType::zeroVal &&
		 start[ 2 ] == TraitType::zeroVal &&
		 start[ 3 ] == TraitType::zeroVal &&
		 start[ 4 ] == TraitType::zeroVal &&
		 start[ 5 ] == TraitType::oneVal &&
		 start[ 6 ] == TraitType::zeroVal &&
		 start[ 7 ] == TraitType::zeroVal &&
		 start[ 8 ] == TraitType::zeroVal &&
		 start[ 9 ] == TraitType::zeroVal &&
		 start[ 10 ] == TraitType::oneVal &&
		 start[ 11 ] == TraitType::zeroVal &&
		 start[ 12 ] == TraitType::zeroVal &&
		 start[ 13 ] == TraitType::zeroVal &&
		 start[ 14 ] == TraitType::zeroVal &&
		 start[ 15 ] == TraitType::oneVal )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////


PNIMATHINLINE
matrix4::
matrix4 ()
{ 
	setIdentity ();
}


PNIMATHINLINE
matrix4::
matrix4 ( InitState )
{ 

}


PNIMATHINLINE
matrix4::
matrix4 ( const ValueType* data )
{ 
	set ( data );
}


PNIMATHINLINE
matrix4::
matrix4 ( const matrix4& matIn )
{ 
	set ( matIn.mat[ 0 ] );
}


PNIMATHINLINE
matrix4::
matrix4 ( ValueType x0y0, ValueType x0y1, ValueType x0y2, ValueType x0y3, 
			ValueType x1y0, ValueType x1y1, ValueType x1y2, ValueType x1y3, 
			ValueType x2y0, ValueType x2y1, ValueType x2y2, ValueType x2y3,
			ValueType x3y0, ValueType x3y1, ValueType x3y2, ValueType x3y3 )
{
	set ( x0y0, x0y1, x0y2, x0y3, 
			x1y0, x1y1, x1y2, x1y3, 
			x2y0, x2y1, x2y2, x2y3,
			x3y0, x3y1, x3y2, x3y3 );
}


PNIMATHINLINE
matrix4::
~matrix4 ()
{ 

}

/////////////////////////////////////////////////////////////////////
		

/////////////////////////////////////////////////////////////////////
// translation method inlines


PNIMATHINLINE
void
matrix4::
setTrans ( const vec3& vecIn )
{ 
	setTrans ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}


PNIMATHINLINE
void
matrix4::
preTrans ( const vec3& vecIn )
{
	preTrans ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}


PNIMATHINLINE
void
matrix4::
postTrans ( const vec3& vecIn )
{
	postTrans ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}


PNIMATHINLINE
bool
matrix4::
getTrans ( vec3& vecIn ) const
{ 
	return getTrans ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////
// rotation method inlines


PNIMATHINLINE
void
matrix4::
setRot ( ValueType degrees, const vec3& vecIn )
{ 
	setRot ( degrees, vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}


PNIMATHINLINE
void
matrix4::
preRot ( ValueType degrees, const vec3& axis )
{
	preRot ( degrees, axis.vec[ 0 ], axis.vec[ 1 ], axis.vec[ 2 ] );
}


PNIMATHINLINE
void
matrix4::
postRot ( ValueType degrees, const vec3& axis )
{
	postRot ( degrees, axis.vec[ 0 ], axis.vec[ 1 ], axis.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////
// quaternion method inlines


PNIMATHINLINE
void
matrix4::
setQuat ( const quat& quat )
{
	setQuat ( quat.vec[ 0 ], quat.vec[ 1 ], quat.vec[ 2 ], quat.vec[ 3 ] );
}


PNIMATHINLINE
void
matrix4::
preQuat ( const quat& quat )
{
	preQuat ( quat.vec[ 0 ], quat.vec[ 1 ], quat.vec[ 2 ], quat.vec[ 3 ] );
}


PNIMATHINLINE
void
matrix4::
postQuat ( const quat& quat )
{
	postQuat ( quat.vec[ 0 ], quat.vec[ 1 ], quat.vec[ 2 ], quat.vec[ 3 ] );
}


PNIMATHINLINE
bool
matrix4::
getQuat ( quat& quat ) const
{
	return getQuat ( quat.vec[ 0 ], quat.vec[ 1 ], quat.vec[ 2 ], quat.vec[ 3 ] );
}

/////////////////////////////////////////////////////////////////////
// euler rotation inlines


PNIMATHINLINE
void
matrix4::
setEuler ( const vec3&  vecIn )
{ 
	setEuler ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}


PNIMATHINLINE
void
matrix4::
preEuler ( const vec3&  vecIn )
{ 
	preEuler ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}


PNIMATHINLINE
void
matrix4::
postEuler ( const vec3&  vecIn )
{ 
	postEuler ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}


PNIMATHINLINE
bool
matrix4::
getEuler ( vec3& vecOut ) const
{ 
	return getEuler ( vecOut.vec[ 0 ], vecOut.vec[ 1 ], vecOut.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
void
matrix4::
set3x3Mat ( const matrix4& rhs )
{
	set ( 
		rhs[ 0 ][ 0 ], rhs[ 0 ][ 1 ], rhs[ 0 ][ 2 ], Trait::zeroVal,
		rhs[ 1 ][ 0 ], rhs[ 1 ][ 1 ], rhs[ 1 ][ 2 ], Trait::zeroVal,
		rhs[ 2 ][ 0 ], rhs[ 2 ][ 1 ], rhs[ 2 ][ 2 ], Trait::zeroVal,
		Trait::zeroVal, Trait::zeroVal, Trait::zeroVal, Trait::oneVal
	);
}

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
void
matrix4::
setScale ( const vec3& vecIn )
{ 
	setScale ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}

PNIMATHINLINE
void
matrix4::
preScale ( const vec3& vecIn )
{ 
	preScale ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}

PNIMATHINLINE
void
matrix4::
postScale ( const vec3& vecIn )
{ 
	postScale ( vecIn.vec[ 0 ], vecIn.vec[ 1 ], vecIn.vec[ 2 ] );
}

PNIMATHINLINE
bool
matrix4::
getScale ( vec3& scaleOut ) const
{ 	
	return getScale ( scaleOut.vec[ 0 ], scaleOut.vec[ 1 ], scaleOut.vec[ 2 ] );
}


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnimatrix4_h
