/////////////////////////////////////////////////////////////////////
//
//	file: vec4.h
//
/////////////////////////////////////////////////////////////////////


#ifndef pnivec4_h
#define pnivec4_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec2.h"
#include "pnivec3.h"
#include "pnivec4.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class quat;
class matrix4;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI vec4
{
	friend class matrix4;
	friend class vec3;

	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef vec4 ThisType;

		enum InitState { NoInit };

		vec4 ();
		vec4 ( InitState );
		vec4 ( ValueType x, ValueType y, ValueType z, ValueType w );
		vec4 ( const ValueType val[ 4 ] );
		vec4 ( const ThisType& vecIn );
		
		void set ( ValueType x, ValueType y, ValueType z, ValueType w );
		void set ( const ValueType val[ 4 ] );
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
		ValueType distHom ( const ThisType& vecIn ) const;
		ValueType distSqr ( const ThisType& vecIn ) const;
		ValueType distSqrHom ( const ThisType& vecIn ) const;
		ValueType length () const;
		ValueType lengthHom () const;
		
		ValueType normalize ();
		void invert ();
		void negate ();
		void negate3 ();

      /// @group Emulate most common glsl swizzling methods
      /// @warning Not meant to be fast path, just for diagnostic/debugging.
    float x () const { return vec[ 0 ]; }
    float y () const { return vec[ 1 ]; }
    float z () const { return vec[ 2 ]; }
    float w () const { return vec[ 3 ]; }
    vec2 xy () const { return vec2 ( vec[ 0 ], vec[ 1 ] ); }
    vec2 yz () const { return vec2 ( vec[ 1 ], vec[ 2 ] ); }
    vec3 xyz () const { return vec3 ( vec[ 0 ], vec[ 1 ], vec[ 2 ] ); }
  
		ValueType dot ( const ThisType& vecIn ) const;
		
		void cross ( const ThisType& aa, const ThisType& bb );
		
		void xformVec ( const ThisType&, const quat& );
		void xformVec ( const ThisType&, const matrix4& );
		void xformPt ( const ThisType&, const matrix4& );

		void projectOnto ( const ThisType& target );
		
    void copyToArray ( ValueType* dst ) const
          { dst[ 0 ] = vec[ 0 ]; dst[ 1 ] = vec[ 1 ]; dst[ 2 ] = vec[ 2 ]; dst[ 3 ] = vec[ 3 ]; }

  
	protected:
		ValueType vec[ 4 ];
				
	private:
		
};

static_assert(sizeof(vec4)==4*sizeof(vec4::ValueType),"sizeof vec4 is not sizeof 4 ValueType elements");
static_assert(std::is_standard_layout<vec4>::value,"vec4 should be standard layout");

/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
void
vec4::
setIdentity ()
{
	vec[ 0 ] = vec[ 1 ] = vec[ 2 ] = vec[ 3 ] = TraitType::zeroVal;
}

PNIMATHINLINE
bool
vec4::isIdentity () const
{
	if ( vec[ 0 ] == TraitType::zeroVal &&
			vec[ 1 ] == TraitType::zeroVal &&
			vec[ 2 ] == TraitType::zeroVal &&
			vec[ 3 ] == TraitType::zeroVal )
		return true;
	else
		return false;
}

PNIMATHINLINE
vec4::
vec4 ()
{
	setIdentity ();
}

PNIMATHINLINE
vec4::
vec4 ( InitState )
{

}

PNIMATHINLINE
vec4::
vec4 ( ValueType x, ValueType y, ValueType z, ValueType w )
{
	vec[ 0 ] = x; vec[ 1 ] = y; vec[ 2 ] = z, vec[ 3 ] = w;
}

PNIMATHINLINE
vec4::
vec4 ( const ValueType val[ 4 ] )
{
	vec[ 0 ] = val[ 0 ]; vec[ 1 ] = val[ 1 ]; vec[ 2 ] = val[ 2 ], vec[ 3 ] = val[ 3 ];
}

PNIMATHINLINE
vec4::
vec4 ( const vec4& vecIn )
{
	vec[ 0 ] = vecIn.vec[ 0 ];
	vec[ 1 ] = vecIn.vec[ 1 ];
	vec[ 2 ] = vecIn.vec[ 2 ];
	vec[ 3 ] = vecIn.vec[ 3 ];
}

PNIMATHINLINE
void
vec4::
set ( ValueType x, ValueType y, ValueType z, ValueType w )
{
	vec[ 0 ] = x; vec[ 1 ] = y; vec[ 2 ] = z; vec[ 3 ] = w;
}

PNIMATHINLINE
void
vec4::
set ( const ValueType val[ 4 ] )
{
	vec[ 0 ] = val[ 0 ]; vec[ 1 ] = val[ 1 ]; 
	vec[ 2 ] = val[ 2 ]; vec[ 3 ] = val[ 3 ];
}

PNIMATHINLINE
const vec4::ValueType&
vec4::
operator[] ( int which ) const
{
	return vec[ which ];
}                

PNIMATHINLINE
vec4::ValueType&
vec4::
operator[] ( int which )
{
	return vec[ which ];
}                

PNIMATHINLINE
vec4& 
vec4::
operator= ( const vec4& vecIn )
{
	vec[ 0 ] = vecIn.vec[ 0 ];
	vec[ 1 ] = vecIn.vec[ 1 ];
	vec[ 2 ] = vecIn.vec[ 2 ]; 
	vec[ 3 ] = vecIn.vec[ 3 ]; 
	return *this;
}

PNIMATHINLINE
bool
vec4::
operator == ( const vec4& vecIn ) const
{
	if (	vec[ 0 ] == vecIn.vec[ 0 ] &&
			vec[ 1 ] == vecIn.vec[ 1 ] &&
			vec[ 2 ] == vecIn.vec[ 2 ] &&
			vec[ 3 ] == vecIn.vec[ 3 ] )
		return true;
	else
		return false;
}

PNIMATHINLINE
bool
vec4::
equal ( const vec4& vecIn, ValueType fuzz ) const
{
	if (	TraitType::equal ( vec[ 0 ], vecIn.vec[ 0 ], fuzz ) &&
			TraitType::equal ( vec[ 1 ], vecIn.vec[ 1 ], fuzz ) &&
			TraitType::equal ( vec[ 2 ], vecIn.vec[ 2 ], fuzz ) &&
			TraitType::equal ( vec[ 3 ], vecIn.vec[ 3 ], fuzz ) )
		return true;
	else
		return false;
}

PNIMATHINLINE
bool 
vec4::
operator < 
( const ThisType& rhs ) const
{
	if ( vec[ 0 ] != rhs.vec[ 0 ] )
		return vec[ 0 ] < rhs.vec[ 0 ];
	if ( vec[ 1 ] != rhs.vec[ 1 ] )
		return vec[ 1 ] < rhs.vec[ 1 ];
	if ( vec[ 2 ] != rhs.vec[ 2 ] )
		return vec[ 2 ] < rhs.vec[ 2 ];
	if ( vec[ 3 ] != rhs.vec[ 3 ] )
		return vec[ 3 ] < rhs.vec[ 3 ];
	return false;
}

PNIMATHINLINE
vec4
vec4::
operator + ( ValueType val ) const
{
	return vec4 (	vec[ 0 ] + val,
						vec[ 1 ] + val,
						vec[ 2 ] + val,
						vec[ 3 ] + val );
}

PNIMATHINLINE
vec4
vec4::
operator + ( const vec4& vecIn ) const
{
	return vec4 (	vec[ 0 ] + vecIn.vec[ 0 ],
						vec[ 1 ] + vecIn.vec[ 1 ],
						vec[ 2 ] + vecIn.vec[ 2 ],
						vec[ 3 ] + vecIn.vec[ 3 ] );
}

PNIMATHINLINE
vec4
vec4::
operator - ( ValueType val ) const
{
	return vec4 (	vec[ 0 ] - val,
						vec[ 1 ] - val,
						vec[ 2 ] - val,
						vec[ 3 ] - val );
}

PNIMATHINLINE
vec4
vec4::
operator - ( const vec4& vecIn ) const
{
	return vec4 (	vec[ 0 ] - vecIn.vec[ 0 ],
						vec[ 1 ] - vecIn.vec[ 1 ],
						vec[ 2 ] - vecIn.vec[ 2 ],
						vec[ 3 ] - vecIn.vec[ 3 ] );
}

PNIMATHINLINE
vec4
vec4::
operator * ( ValueType val ) const
{
	return vec4 (	vec[ 0 ] * val,
						vec[ 1 ] * val,
						vec[ 2 ] * val,
						vec[ 3 ] * val );
}

PNIMATHINLINE
vec4
vec4::
operator * ( const vec4& vecIn ) const
{
	return vec4 (	vec[ 0 ] * vecIn.vec[ 0 ],
						vec[ 1 ] * vecIn.vec[ 1 ],
						vec[ 2 ] * vecIn.vec[ 2 ],
						vec[ 3 ] * vecIn.vec[ 3 ] );
}

PNIMATHINLINE
vec4
vec4::
operator / ( ValueType val ) const
{
	return vec4 (	vec[ 0 ] / val,
						vec[ 1 ] / val,
						vec[ 2 ] / val,
						vec[ 3 ] / val );
}

PNIMATHINLINE
vec4
vec4::
operator / ( const vec4& vecIn ) const
{
	return vec4 (	vec[ 0 ] / vecIn.vec[ 0 ],
						vec[ 1 ] / vecIn.vec[ 1 ],
						vec[ 2 ] / vecIn.vec[ 2 ],
						vec[ 3 ] / vecIn.vec[ 3 ] );
}

PNIMATHINLINE
vec4&
vec4::
operator += ( ValueType val )
{
	vec[ 0 ] += val;
	vec[ 1 ] += val;
	vec[ 2 ] += val; 
	vec[ 3 ] += val; 
	return *this;
}

PNIMATHINLINE
vec4&
vec4::
operator += ( const vec4& vecIn )
{
	vec[ 0 ] += vecIn.vec[ 0 ];
	vec[ 1 ] += vecIn.vec[ 1 ];
	vec[ 2 ] += vecIn.vec[ 2 ]; 
	vec[ 3 ] += vecIn.vec[ 3 ]; 
	return *this;
}

PNIMATHINLINE
vec4&
vec4::
operator -= ( ValueType val )
{
	vec[ 0 ] -= val;
	vec[ 1 ] -= val;
	vec[ 2 ] -= val;
	vec[ 3 ] -= val;
	return *this;
}

PNIMATHINLINE
vec4&
vec4::
operator -= ( const vec4& vecIn )
{
	vec[ 0 ] -= vecIn.vec[ 0 ];
	vec[ 1 ] -= vecIn.vec[ 1 ];
	vec[ 2 ] -= vecIn.vec[ 2 ]; 
	vec[ 3 ] -= vecIn.vec[ 3 ]; 
	return *this;
}

PNIMATHINLINE
vec4&
vec4::
operator *= ( ValueType val )
{
	vec[ 0 ] *= val;
	vec[ 1 ] *= val;
	vec[ 2 ] *= val; 
	vec[ 3 ] *= val; 
	return *this;
}

PNIMATHINLINE
vec4&
vec4::
operator *= ( const vec4& vecIn )
{
	vec[ 0 ] *= vecIn.vec[ 0 ];
	vec[ 1 ] *= vecIn.vec[ 1 ];
	vec[ 2 ] *= vecIn.vec[ 2 ];
	vec[ 3 ] *= vecIn.vec[ 3 ];
	return *this;
}

PNIMATHINLINE
vec4&
vec4::
operator /= ( ValueType val )
{
	vec[ 0 ] /= val;
	vec[ 1 ] /= val;
	vec[ 2 ] /= val; 
	vec[ 3 ] /= val; 
	return *this;
}

PNIMATHINLINE
void
vec4::
combine ( ValueType t1, const vec4& v1, ValueType t2, const vec4& v2 )
{
	vec[ 0 ] = t1 * v1.vec[ 0 ] + t2 * v2.vec[ 0 ];
	vec[ 1 ] = t1 * v1.vec[ 1 ] + t2 * v2.vec[ 1 ];
	vec[ 2 ] = t1 * v1.vec[ 2 ] + t2 * v2.vec[ 2 ];
	vec[ 3 ] = t1 * v1.vec[ 3 ] + t2 * v2.vec[ 3 ];
}

PNIMATHINLINE
vec4&
vec4::
operator /= ( const vec4& vecIn )
{
	vec[ 0 ] /= vecIn.vec[ 0 ];
	vec[ 1 ] /= vecIn.vec[ 1 ];
	vec[ 2 ] /= vecIn.vec[ 2 ]; 
	vec[ 3 ] /= vecIn.vec[ 3 ]; 
	return *this;
}

PNIMATHINLINE
vec4::ValueType
vec4::
normalize ()
{
	ValueType len = length ();
	*this /= len;
	return len;
}

PNIMATHINLINE
void
vec4::
invert ()
{
	vec[ 0 ] = TraitType::oneVal / vec[ 0 ];
	vec[ 1 ] = TraitType::oneVal / vec[ 1 ];
	vec[ 2 ] = TraitType::oneVal / vec[ 2 ];
	vec[ 3 ] = TraitType::oneVal / vec[ 3 ];
}

PNIMATHINLINE
void
vec4::
negate ()
{
	vec[ 0 ] = -vec[ 0 ];
	vec[ 1 ] = -vec[ 1 ];
	vec[ 2 ] = -vec[ 2 ];
	vec[ 3 ] = -vec[ 3 ];
}

PNIMATHINLINE
void
vec4::
negate3 ()
{
	vec[ 0 ] = -vec[ 0 ];
	vec[ 1 ] = -vec[ 1 ];
	vec[ 2 ] = -vec[ 2 ];
}

PNIMATHINLINE
vec4::ValueType
vec4::
dot ( const vec4& vecIn ) const
{
	return	vecIn.vec[ 0 ] * vec[ 0 ] +
			vecIn.vec[ 1 ] * vec[ 1 ] +
			vecIn.vec[ 2 ] * vec[ 2 ] +
			vecIn.vec[ 3 ] * vec[ 3 ];
}

PNIMATHINLINE
void
vec4::
cross ( const vec4& aa, const vec4& bb )
{
//	vec[ 0 ] = aa.vec[ 1 ] * bb.vec[ 2 ] -
//			aa.vec[ 2 ] * bb.vec[ 1 ];
//	vec[ 1 ] = aa.vec[ 2 ] * bb.vec[ 0 ] -
//			aa.vec[ 0 ] * bb.vec[ 2 ];
//	vec[ 2 ] = aa.vec[ 0 ] * bb.vec[ 1 ] -
//			aa.vec[ 1 ] * bb.vec[ 0 ];
}

PNIMATHINLINE
vec4::ValueType
vec4::
distSqr ( const vec4& vecIn ) const
{
        return  
                ( vec[ 0 ] - vecIn.vec[ 0 ] ) * ( vec[ 0 ] - vecIn.vec[ 0 ] ) +
                ( vec[ 1 ] - vecIn.vec[ 1 ] ) * ( vec[ 1 ] - vecIn.vec[ 1 ] ) +
                ( vec[ 2 ] - vecIn.vec[ 2 ] ) * ( vec[ 2 ] - vecIn.vec[ 2 ] ) +
                ( vec[ 3 ] - vecIn.vec[ 3 ] ) * ( vec[ 3 ] - vecIn.vec[ 3 ] );
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnivec4_h


