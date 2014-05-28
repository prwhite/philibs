/////////////////////////////////////////////////////////////////////
//
//	file: vec2.h
//
/////////////////////////////////////////////////////////////////////


#ifndef pnivec2_h
#define pnivec2_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

		class box2;

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI vec2
{
	friend class box2;

	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef vec2 ThisType;
	
		enum InitState { NoInit };
		
		vec2 ();
		vec2 ( InitState );
		vec2 ( ValueType x, ValueType y );
		vec2 ( const ValueType val[ 2 ] );
		vec2 ( const ThisType& vecIn );
				
		void set ( ValueType x, ValueType y );
		void set ( const ValueType val[ 2 ] );

		void setIdentity ();
		bool isIdentity () const;		// exact, not a fuzz compare
				
		const ValueType& operator[] ( int which ) const;           
		operator ValueType* () { return vec; }
		operator const ValueType* () const { return vec; }
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

		void projectOnto ( const ThisType& target );

    void copyToArray ( ValueType* dst ) const
        { dst[ 0 ] = vec[ 0 ]; dst[ 1 ] = vec[ 1 ]; }
		
//		void cross ( const ThisType& aa, const ThisType& bb );
//		void xformVec ( const ThisType&, const matrix4& );
//		void xformPt ( const ThisType&, const matrix4& );
				
	protected:
		ValueType vec[ 2 ];
		
	private:
		
};

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
void
vec2::
setIdentity ()
{
	vec[ 0 ] = vec[ 1 ] = TraitType::zeroVal;
}

PNIMATHINLINE
bool
vec2::isIdentity () const
{
	if ( vec[ 0 ] == TraitType::zeroVal &&
			vec[ 1 ] == TraitType::zeroVal )
		return true;
	else
		return false;
}

PNIMATHINLINE
vec2::
vec2 ()
{
	setIdentity ();
}

PNIMATHINLINE
vec2::
vec2 ( InitState )
{

}

PNIMATHINLINE
vec2::
vec2 ( ValueType x, ValueType y )
{
	vec[ 0 ] = x; vec[ 1 ] = y;
}

PNIMATHINLINE
vec2::
vec2 ( const ValueType val[ 2 ] )
{
	vec[ 0 ] = val [ 0 ]; vec[ 1 ] = val [ 1 ];
}

PNIMATHINLINE
vec2::
vec2 ( const vec2& vecIn )
{
	vec[ 0 ] = vecIn.vec[ 0 ];
	vec[ 1 ] = vecIn.vec[ 1 ];
}

PNIMATHINLINE
void
vec2::
set ( ValueType x, ValueType y )
{
	vec[ 0 ] = x; vec[ 1 ] = y;
}

PNIMATHINLINE
void
vec2::
set ( const ValueType val[ 2 ] )
{
	vec[ 0 ] = val[ 0 ]; vec[ 1 ] = val[ 1 ];
}

PNIMATHINLINE
const vec2::ValueType&
vec2::
operator[] ( int which ) const
{
	return vec[ which ];
}                

PNIMATHINLINE
vec2::ValueType&
vec2::
operator[] ( int which )
{
	return vec[ which ];
}                

PNIMATHINLINE
vec2& 
vec2::
operator= ( const vec2& vecIn )
{
	vec[ 0 ] = vecIn.vec[ 0 ];
	vec[ 1 ] = vecIn.vec[ 1 ];
	return *this;
}

PNIMATHINLINE
bool
vec2::
operator == ( const vec2& vecIn ) const
{
	if (	vec[ 0 ] == vecIn.vec[ 0 ] &&
			vec[ 1 ] == vecIn.vec[ 1 ] )
		return true;
	else
		return false;
}



PNIMATHINLINE
bool
vec2::
equal ( const vec2& vecIn, ValueType fuzz ) const
{
	if (	TraitType::equal ( vec[ 0 ], vecIn.vec[ 0 ], fuzz ) &&
			TraitType::equal ( vec[ 1 ], vecIn.vec[ 1 ], fuzz ) )
		return true;
	else
		return false;
}

PNIMATHINLINE
bool 
vec2::
operator < 
( const ThisType& rhs ) const
{
	if ( vec[ 0 ] != rhs.vec[ 0 ] )
		return vec[ 0 ] < rhs.vec[ 0 ];
	if ( vec[ 1 ] != rhs.vec[ 1 ] )
		return vec[ 1 ] < rhs.vec[ 1 ];
	return false;
}

PNIMATHINLINE
vec2
vec2::
operator + ( ValueType val ) const
{
	return vec2 (	vec[ 0 ] + val,
						vec[ 1 ] + val );
}

PNIMATHINLINE
vec2
vec2::
operator + ( const vec2& vecIn ) const
{
	return vec2 (	vec[ 0 ] + vecIn.vec[ 0 ],
						vec[ 1 ] + vecIn.vec[ 1 ] );
}

PNIMATHINLINE
vec2
vec2::
operator - ( ValueType val ) const
{
	return vec2 (	vec[ 0 ] - val,
						vec[ 1 ] - val );
}

PNIMATHINLINE
vec2
vec2::
operator - ( const vec2& vecIn ) const
{
	return vec2 (	vec[ 0 ] - vecIn.vec[ 0 ],
						vec[ 1 ] - vecIn.vec[ 1 ] );
}

PNIMATHINLINE
vec2
vec2::
operator * ( ValueType val ) const
{
	return vec2 (	vec[ 0 ] * val,
						vec[ 1 ] * val );
}

PNIMATHINLINE
vec2
vec2::
operator * ( const vec2& vecIn ) const
{
	return vec2 (	vec[ 0 ] * vecIn.vec[ 0 ],
						vec[ 1 ] * vecIn.vec[ 1 ] );
}

PNIMATHINLINE
vec2
vec2::
operator / ( ValueType val ) const
{
	return vec2 (	vec[ 0 ] / val,
						vec[ 1 ] / val );
}

PNIMATHINLINE
vec2
vec2::
operator / ( const vec2& vecIn ) const
{
	return vec2 (	vec[ 0 ] / vecIn.vec[ 0 ],
						vec[ 1 ] / vecIn.vec[ 1 ] );
}


PNIMATHINLINE
vec2&
vec2::
operator += ( ValueType val )
{
	vec[ 0 ] += val;
	vec[ 1 ] += val;
	return *this;
}

PNIMATHINLINE
vec2&
vec2::
operator += ( const vec2& vecIn )
{
	vec[ 0 ] += vecIn.vec[ 0 ];
	vec[ 1 ] += vecIn.vec[ 1 ];
	return *this;
}

PNIMATHINLINE
vec2&
vec2::
operator -= ( ValueType val )
{
	vec[ 0 ] -= val;
	vec[ 1 ] -= val;
	return *this;
}

PNIMATHINLINE
vec2&
vec2::
operator -= ( const vec2& vecIn )
{
	vec[ 0 ] -= vecIn.vec[ 0 ];
	vec[ 1 ] -= vecIn.vec[ 1 ];
	return *this;
}

PNIMATHINLINE
vec2&
vec2::
operator *= ( ValueType val )
{
	vec[ 0 ] *= val;
	vec[ 1 ] *= val;
	return *this;
}

PNIMATHINLINE
vec2&
vec2::
operator *= ( const vec2& vecIn )
{
	vec[ 0 ] *= vecIn.vec[ 0 ];
	vec[ 1 ] *= vecIn.vec[ 1 ];
	return *this;
}

PNIMATHINLINE
vec2&
vec2::
operator /= ( ValueType val )
{
	vec[ 0 ] /= val;
	vec[ 1 ] /= val;
	return *this;
}

PNIMATHINLINE
void
vec2::
combine ( ValueType t1, const vec2& v1, ValueType t2, const vec2& v2 )
{
	vec[ 0 ] = t1 * v1.vec[ 0 ] + t2 * v2.vec[ 0 ];
	vec[ 1 ] = t1 * v1.vec[ 1 ] + t2 * v2.vec[ 1 ];
}

PNIMATHINLINE
vec2&
vec2::
operator /= ( const vec2& vecIn )
{
	vec[ 0 ] /= vecIn.vec[ 0 ];
	vec[ 1 ] /= vecIn.vec[ 1 ];
	return *this;
}

PNIMATHINLINE
vec2::ValueType
vec2::
normalize ()
{
	ValueType len = length ();
	*this /= len;
	return len;
}

PNIMATHINLINE
void
vec2::
invert ()
{
	vec[ 0 ] = TraitType::oneVal / vec[ 0 ];
	vec[ 1 ] = TraitType::oneVal / vec[ 1 ];
}

PNIMATHINLINE
void
vec2::
negate ()
{
	vec[ 0 ] = -vec[ 0 ];
	vec[ 1 ] = -vec[ 1 ];
}

PNIMATHINLINE
vec2::ValueType
vec2::
dot ( const vec2& vecIn ) const
{
	return	vecIn.vec[ 0 ] * vec[ 0 ] +
			vecIn.vec[ 1 ] * vec[ 1 ];
}

PNIMATHINLINE
vec2::ValueType
vec2::
distSqr ( const vec2& vecIn ) const
{
        return  
                ( vec[ 0 ] - vecIn.vec[ 0 ] ) * ( vec[ 0 ] - vecIn.vec[ 0 ] ) +
                ( vec[ 1 ] - vecIn.vec[ 1 ] ) * ( vec[ 1 ] - vecIn.vec[ 1 ] );
}


/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnivec2_h

