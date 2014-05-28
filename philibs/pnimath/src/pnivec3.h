/////////////////////////////////////////////////////////////////////
//
//	file: vec3.h
//
/////////////////////////////////////////////////////////////////////


#ifndef pnivec3_h
#define pnivec3_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class quat;
class matrix4;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI vec3
{
	friend class matrix4;
	friend class vec4;
	friend class quat;
	friend class box3;
	friend class sphere;
	friend class plane;

	public:
			//mtcl: typedef float TraitType::ValueType;
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef vec3 ThisType;

		enum InitState { NoInit };
		
		vec3 ();
		vec3 ( InitState );
		vec3 ( ValueType x, ValueType y, ValueType z );
		vec3 ( const ValueType val[ 3 ] );
		vec3 ( const vec3& vecIn );
				
		void set ( ValueType x, ValueType y, ValueType z );
		void set ( const ValueType val[ 3 ] );
		operator ValueType* () { return vec; }
		operator const ValueType* () const { return vec; }
		void setIdentity ();
		bool isIdentity () const;		// exact, not a fuzz compare
						
		const ValueType& operator[] ( int which ) const;           
		ValueType& operator[] ( int which );           
		
		ThisType& operator= ( const ThisType& vecIn );
		bool operator == ( const ThisType& vecIn ) const;
		bool equal ( const ThisType& vecIn, ValueType fuzz = TraitType::fuzzVal ) const;
	
		bool operator < ( const ThisType& rhs ) const;
	
		ThisType operator + ( ValueType val ) const;
		ThisType operator + ( const ThisType& vecIn ) const;
		ThisType operator - ( ValueType val ) const;
		ThisType operator - ( const ThisType& vecIn ) const;
		ThisType operator * ( ValueType val ) const;
		ThisType operator * ( const ThisType& vecIn ) const;
		ThisType operator / ( ValueType val ) const;
		ThisType operator / ( const ThisType& vecIn ) const;
	
		ThisType& operator += ( ValueType val );
		ThisType& operator += ( const ThisType& vecIn );
		ThisType& operator -= ( ValueType val );
		ThisType& operator -= ( const ThisType& vecIn );
		ThisType& operator *= ( ValueType val );
		ThisType& operator *= ( const ThisType& vecIn );
		ThisType& operator /= ( ValueType val );
		ThisType& operator /= ( const ThisType& vecIn );
	
		void combine ( ValueType t1, const ThisType& v1, ValueType t2, const ThisType& v2 );
		ValueType dist ( const ThisType& vecIn ) const;
		ValueType distSqr ( const ThisType& vecIn ) const;
		ValueType length () const;
		ValueType normalize ();
		void invert ();
		void negate ();
		
		ValueType dot ( const ThisType& vecIn ) const;
		
		void cross ( const ThisType& aa, const ThisType& bb );
		
		void xformVec ( const ThisType&, const matrix4& );
		void xformVec4 ( const ThisType&, const matrix4& );	// full xform
		void xformVec ( const ThisType&, const quat& );
		void xformPt ( const ThisType&, const matrix4& );
		void xformPt4 ( const ThisType&, const matrix4& );	// full xform
		
		ValueType getAxisAngle ( const ThisType& v1, const ThisType& v2 );

      /// Project this as a vector onto target as a vector (as opposed to
      /// representations of points, naturally).
      /// @note http://en.wikipedia.org/wiki/Vector_projection
		void projectOnto ( const ThisType& target );
  
    void copyToArray ( ValueType* dst ) const
        { dst[ 0 ] = vec[ 0 ]; dst[ 1 ] = vec[ 1 ]; dst[ 2 ] = vec[ 2 ]; }
		
	protected:
		ValueType vec[ 3 ];
				
	private:
		
};

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
void
vec3::
setIdentity ()
{
	vec[ 0 ] = vec[ 1 ] = vec[ 2 ] = TraitType::zeroVal;
}

PNIMATHINLINE
bool
vec3::isIdentity () const
{
	if ( vec[ 0 ] == TraitType::zeroVal &&
			vec[ 1 ] == TraitType::zeroVal &&
			vec[ 2 ] == TraitType::zeroVal )
		return true;
	else
		return false;
}

PNIMATHINLINE
vec3::
vec3 ()
{
	setIdentity ();
}

PNIMATHINLINE
vec3::
vec3 ( InitState )
{

}

PNIMATHINLINE
vec3::
vec3 ( ValueType x, ValueType y, ValueType z )
{
	vec[ 0 ] = x; vec[ 1 ] = y; vec[ 2 ] = z;
}

PNIMATHINLINE
vec3::
vec3 ( const ValueType val[ 3 ] )
{
	vec[ 0 ] = val[ 0 ]; vec[ 1 ] = val[ 1 ]; vec[ 2 ] = val[ 2 ];
}

PNIMATHINLINE
vec3::
vec3 ( const vec3& vecIn )
{
	vec[ 0 ] = vecIn.vec[ 0 ];
	vec[ 1 ] = vecIn.vec[ 1 ];
	vec[ 2 ] = vecIn.vec[ 2 ];
}

PNIMATHINLINE
void
vec3::
set ( ValueType x, ValueType y, ValueType z )
{
	vec[ 0 ] = x; vec[ 1 ] = y; vec[ 2 ] = z;
}

PNIMATHINLINE
void
vec3::
set ( const ValueType val[ 3 ] )
{
	vec[ 0 ] = val[ 0 ]; vec[ 1 ] = val[ 1 ]; vec[ 2 ] = val[ 2 ];
}

PNIMATHINLINE
const vec3::ValueType&
vec3::
operator[] ( int which ) const
{
	return vec[ which ];
}                

PNIMATHINLINE
vec3::ValueType&
vec3::
operator[] ( int which )
{
	return vec[ which ];
}                

PNIMATHINLINE
vec3& 
vec3::
operator= ( const vec3& vecIn )
{
	vec[ 0 ] = vecIn.vec[ 0 ];
	vec[ 1 ] = vecIn.vec[ 1 ];
	vec[ 2 ] = vecIn.vec[ 2 ]; 
	return *this;
}

PNIMATHINLINE
bool
vec3::
operator == ( const vec3& vecIn ) const
{
	if (	vec[ 0 ] == vecIn.vec[ 0 ] &&
			vec[ 1 ] == vecIn.vec[ 1 ] &&
			vec[ 2 ] == vecIn.vec[ 2 ] )
		return true;
	else
		return false;
}

PNIMATHINLINE
bool
vec3::
equal ( const vec3& vecIn, ValueType fuzz ) const
{
	if (	TraitType::equal ( vec[ 0 ], vecIn.vec[ 0 ], fuzz ) &&
			TraitType::equal ( vec[ 1 ], vecIn.vec[ 1 ], fuzz ) &&
			TraitType::equal ( vec[ 2 ], vecIn.vec[ 2 ], fuzz ) )
		return true;
	else
		return false;
}

PNIMATHINLINE
bool 
vec3::
operator < 
( const ThisType& rhs ) const
{
	if ( vec[ 0 ] != rhs.vec[ 0 ] )
		return vec[ 0 ] < rhs.vec[ 0 ];
	if ( vec[ 1 ] != rhs.vec[ 1 ] )
		return vec[ 1 ] < rhs.vec[ 1 ];
	if ( vec[ 2 ] != rhs.vec[ 2 ] )
		return vec[ 2 ] < rhs.vec[ 2 ];
	return false;
}

PNIMATHINLINE
vec3
vec3::
operator + ( ValueType val ) const
{
	return vec3 (	vec[ 0 ] + val,
						vec[ 1 ] + val,
						vec[ 2 ] + val );
}

PNIMATHINLINE
vec3
vec3::
operator + ( const vec3& vecIn ) const
{
	return vec3 (	vec[ 0 ] + vecIn.vec[ 0 ],
						vec[ 1 ] + vecIn.vec[ 1 ],
						vec[ 2 ] + vecIn.vec[ 2 ] );
}

PNIMATHINLINE
vec3
vec3::
operator - ( ValueType val ) const
{
	return vec3 (	vec[ 0 ] - val,
						vec[ 1 ] - val,
						vec[ 2 ] - val );
}

PNIMATHINLINE
vec3
vec3::
operator - ( const vec3& vecIn ) const
{
	return vec3 (	vec[ 0 ] - vecIn.vec[ 0 ],
						vec[ 1 ] - vecIn.vec[ 1 ],
						vec[ 2 ] - vecIn.vec[ 2 ] );
}

PNIMATHINLINE
vec3
vec3::
operator * ( ValueType val ) const
{
	return vec3 (	vec[ 0 ] * val,
						vec[ 1 ] * val,
						vec[ 2 ] * val );
}

PNIMATHINLINE
vec3
vec3::
operator * ( const vec3& vecIn ) const
{
	return vec3 (	vec[ 0 ] * vecIn.vec[ 0 ],
						vec[ 1 ] * vecIn.vec[ 1 ],
						vec[ 2 ] * vecIn.vec[ 2 ] );
}

PNIMATHINLINE
vec3
vec3::
operator / ( ValueType val ) const
{
	return vec3 (	vec[ 0 ] / val,
						vec[ 1 ] / val,
						vec[ 2 ] / val );
}

PNIMATHINLINE
vec3
vec3::
operator / ( const vec3& vecIn ) const
{
	return vec3 (	vec[ 0 ] / vecIn.vec[ 0 ],
						vec[ 1 ] / vecIn.vec[ 1 ],
						vec[ 2 ] / vecIn.vec[ 2 ] );
}


PNIMATHINLINE
vec3&
vec3::
operator += ( ValueType val )
{
	vec[ 0 ] += val;
	vec[ 1 ] += val;
	vec[ 2 ] += val; 
	return *this;
}

PNIMATHINLINE
vec3&
vec3::
operator += ( const vec3& vecIn )
{
	vec[ 0 ] += vecIn.vec[ 0 ];
	vec[ 1 ] += vecIn.vec[ 1 ];
	vec[ 2 ] += vecIn.vec[ 2 ]; 
	return *this;
}

PNIMATHINLINE
vec3&
vec3::
operator -= ( ValueType val )
{
	vec[ 0 ] -= val;
	vec[ 1 ] -= val;
	vec[ 2 ] -= val;
	return *this;
}

PNIMATHINLINE
vec3&
vec3::
operator -= ( const vec3& vecIn )
{
	vec[ 0 ] -= vecIn.vec[ 0 ];
	vec[ 1 ] -= vecIn.vec[ 1 ];
	vec[ 2 ] -= vecIn.vec[ 2 ]; 
	return *this;
}

PNIMATHINLINE
vec3&
vec3::
operator *= ( ValueType val )
{
	vec[ 0 ] *= val;
	vec[ 1 ] *= val;
	vec[ 2 ] *= val; 
	return *this;
}

PNIMATHINLINE
vec3&
vec3::
operator *= ( const vec3& vecIn )
{
	vec[ 0 ] *= vecIn.vec[ 0 ];
	vec[ 1 ] *= vecIn.vec[ 1 ];
	vec[ 2 ] *= vecIn.vec[ 2 ];
	return *this;
}

PNIMATHINLINE
vec3&
vec3::
operator /= ( ValueType val )
{
	vec[ 0 ] /= val;
	vec[ 1 ] /= val;
	vec[ 2 ] /= val; 
	return *this;
}

PNIMATHINLINE
vec3&
vec3::
operator /= ( const vec3& vecIn )
{
	vec[ 0 ] /= vecIn.vec[ 0 ];
	vec[ 1 ] /= vecIn.vec[ 1 ];
	vec[ 2 ] /= vecIn.vec[ 2 ]; 
	return *this;
}

PNIMATHINLINE
void
vec3::
combine ( ValueType t1, const vec3& v1, ValueType t2, const vec3& v2 )
{
	vec[ 0 ] = t1 * v1.vec[ 0 ] + t2 * v2.vec[ 0 ];
	vec[ 1 ] = t1 * v1.vec[ 1 ] + t2 * v2.vec[ 1 ];
	vec[ 2 ] = t1 * v1.vec[ 2 ] + t2 * v2.vec[ 2 ];
}

PNIMATHINLINE
vec3::ValueType
vec3::
normalize ()
{
	ValueType len = length ();
	*this /= len;
	return len;
}

PNIMATHINLINE
void
vec3::
invert ()
{
	vec[ 0 ] = TraitType::oneVal / vec[ 0 ];
	vec[ 1 ] = TraitType::oneVal / vec[ 1 ];
	vec[ 2 ] = TraitType::oneVal / vec[ 2 ];
}

PNIMATHINLINE
void
vec3::
negate ()
{
	vec[ 0 ] = -vec[ 0 ];
	vec[ 1 ] = -vec[ 1 ];
	vec[ 2 ] = -vec[ 2 ];
}

PNIMATHINLINE
vec3::ValueType
vec3::
dot ( const vec3& vecIn ) const
{
	return	vecIn.vec[ 0 ] * vec[ 0 ] +
			vecIn.vec[ 1 ] * vec[ 1 ] +
			vecIn.vec[ 2 ] * vec[ 2 ];
}

PNIMATHINLINE
void
vec3::
cross ( const vec3& aa, const vec3& bb )
{
	vec[ 0 ] = aa.vec[ 1 ] * bb.vec[ 2 ] -
			aa.vec[ 2 ] * bb.vec[ 1 ];
	vec[ 1 ] = aa.vec[ 2 ] * bb.vec[ 0 ] -
			aa.vec[ 0 ] * bb.vec[ 2 ];
	vec[ 2 ] = aa.vec[ 0 ] * bb.vec[ 1 ] -
			aa.vec[ 1 ] * bb.vec[ 0 ];
}

PNIMATHINLINE
vec3::ValueType
vec3::
distSqr ( const vec3& vecIn ) const
{
        return  
                ( vec[ 0 ] - vecIn.vec[ 0 ] ) * ( vec[ 0 ] - vecIn.vec[ 0 ] ) +
                ( vec[ 1 ] - vecIn.vec[ 1 ] ) * ( vec[ 1 ] - vecIn.vec[ 1 ] ) +
                ( vec[ 2 ] - vecIn.vec[ 2 ] ) * ( vec[ 2 ] - vecIn.vec[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnivec3_h




