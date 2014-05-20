// ///////////////////////////////////////////////////////////////////
//
//	class: seg
//
// ///////////////////////////////////////////////////////////////////


#ifndef pniseg_h
#define pniseg_h

// ///////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec3.h"

// ///////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

// ///////////////////////////////////////////////////////////////////

/**
  The seg is a representation of a line with a beginning and end point.
  It is stored as the origin (vec3) of the segment, the normalized direction 
  (vec3) the segment points in, and the length (float).  
  @note This internal representation differs from another common representation 
  which has a non-normalized direction and  a length which can be treated as
  a paramemtric value ranging from 0 to 1.
*/

class PNIMATHAPI seg 
{
	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef seg ThisType;

		enum InitState { NoInit };

		seg ();
		seg ( InitState );
		seg ( const vec3& posIn, const vec3& dirIn, ValueType lengthIn );
		seg ( const vec3& posIn, const vec3& endIn );
		seg ( const vec3& posIn, ValueType headingIn, ValueType pitchIn, 
				ValueType lengthIn );
		seg ( const seg& orig );
		~seg ();
		
		// set methods
		seg& operator = ( const seg& orig );
		void set ( const vec3& posIn, const vec3& dirIn, ValueType lengthIn );
		void set ( const vec3& posIn, const vec3& endIn );
		void set ( const vec3& posIn, ValueType headingIn, ValueType pitchIn, 
				ValueType lengthIn );

		// get methods		
		void get ( vec3& posOut, vec3& dirOut, ValueType& lengthOut ) const;
		void get ( vec3& begOut, vec3& endOut ) const;
		void getPos ( vec3& posOut ) const;
		void getDir ( vec3& dirOut ) const;
		const vec3& getPos () const;
		const vec3& getDir () const;
		void setLength ( ValueType val );
		ValueType getLength () const;

		// equality methods
		bool operator == ( const ThisType& vecIn ) const;
		bool equal ( const ThisType& vecIn, ValueType fuzz = TraitType::fuzzVal ) const;

		// get the closest point to pt in the primative
		bool closestPtIn ( vec3& dest, 
				ValueType xval, ValueType yval, ValueType zval ) const;
		bool closestPtIn ( vec3& dest, const vec3& pt ) const;

		// Point contains tests.
		int contains ( ValueType x, ValueType y, ValueType z ) const;
		int contains ( const vec3& pt ) const;
		int contains ( const seg& segIn ) const;


		// extendBy ()
			//mtcl: mtcl_begin_ignore
		bool extendBy ( ValueType x, ValueType y, ValueType c );
		bool extendBy ( const vec3& pt );
			//mtcl: mtcl_end_ignore

      /// Find intersection of two segs, or none.
    bool isect ( ThisType const& rhs, vec3& dst ) const;

		// simple point interp method
		// this interpolates along the segment given the range [0,1]
		vec3 lerp ( ValueType value ) const;
		void lerp ( vec3& dest, ValueType value ) const;
		
		// xform methods
		void xform ( const seg& seg, const matrix4& mat );
		void xform4 ( const seg& seg, const matrix4& mat );

	protected:
		vec3	pos;
		vec3	dir;          /// Always normalized
		ValueType	length;   /// Actual length, not 0 <= length <= 1

	private:

};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

PNIMATHINLINE
seg::
seg () :
	length ( TraitType::zeroVal )
{
	
}


PNIMATHINLINE
seg::
seg ( InitState ) :
	pos ( vec3::NoInit ), 
	dir ( vec3::NoInit )
{
}



PNIMATHINLINE
seg::
seg ( const vec3& posIn, const vec3& dirIn, ValueType lengthIn ) :
	pos ( posIn ), 
	dir ( dirIn ), 
	length ( lengthIn )
{
}



PNIMATHINLINE
seg::
seg ( const vec3& posIn, const vec3& endIn )
{
	set ( posIn, endIn );
}



PNIMATHINLINE
seg::
seg ( const vec3& posIn, ValueType headingIn, ValueType pitchIn, 
		ValueType lengthIn )
{
	set ( posIn, headingIn, pitchIn, lengthIn );
}



PNIMATHINLINE
seg::
seg ( const seg& orig ) :
	pos ( orig.pos ), 
	dir ( orig.dir ), 
	length ( orig.length )
{
}



PNIMATHINLINE
seg::
~seg ()
{
}



/////////////////////////////////////////////////////////////////////
// set methods

PNIMATHINLINE
seg&
seg::
operator = ( const seg& orig )
{
	pos = orig.pos;
	dir = orig.dir;
	length = orig.length;
	
	return *this;
}



PNIMATHINLINE
void
seg::
set ( const vec3& posIn, const vec3& dirIn, ValueType lengthIn )
{
	pos = posIn;
	dir = dirIn;
	length = lengthIn;
}


/////////////////////////////////////////////////////////////////////
// get methods		

PNIMATHINLINE
void
seg::
get ( vec3& posOut, vec3& dirOut, ValueType& lengthOut ) const
{
	posOut = pos;
	dirOut = dir;
	lengthOut = length;
}

PNIMATHINLINE
void
seg::
get ( vec3& begOut, vec3& endOut ) const
{
	begOut = pos;
	endOut = dir;
	endOut *= length;
	endOut += begOut;
}

PNIMATHINLINE
void
seg::
getPos ( vec3& posOut ) const
{
	posOut = pos;
}

PNIMATHINLINE
void
seg::
getDir ( vec3& dirOut ) const
{
	dirOut = dir;
}

PNIMATHINLINE
const vec3&
seg::
getPos () const
{
	return pos;
}

PNIMATHINLINE
const vec3&
seg::
getDir () const
{
	return dir;
}

PNIMATHINLINE
void
seg::
setLength ( ValueType val )
{
	length = val;
}

PNIMATHINLINE
seg::ValueType
seg::
getLength () const
{
	return length;
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pniseg_h
