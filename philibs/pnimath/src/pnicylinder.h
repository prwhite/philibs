/////////////////////////////////////////////////////////////////////
//
//	class: cylinder
//
/////////////////////////////////////////////////////////////////////


#ifndef pnicylinder_h
#define pnicylinder_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec3.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class box3;
class seg;
class sphere;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI cylinder 
{
	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef cylinder ThisType;

		enum InitState { NoInit };

		cylinder ();
		cylinder ( const vec3& centerIn, ValueType radiusIn, const vec3& axisIn, ValueType halfLengthIn );
		cylinder ( InitState );
		cylinder ( const cylinder& orig );
		~cylinder ();

		cylinder& operator = ( const cylinder& orig );
		
		// set methods
		void set ( const vec3& centerIn, ValueType radiusIn, const vec3& axisIn, ValueType halfLengthIn );
		void setCenter ( const vec3& centerIn );
		void setRadius ( ValueType radiusIn );
		void setAxis ( const vec3& axisIn );
		void setHalfLength ( ValueType halfLengthIn );

		// get methods
		void get ( vec3& centerOut, ValueType& radiusOut, vec3& axisOut, ValueType& halfLengthOut ) const;
		void getCenter ( vec3& centerIn ) const;
		ValueType getRadius () const;
		void getAxis ( vec3& axisIn ) const;
		ValueType getHalfLength () const;

		// equality methods
		bool operator == ( const ThisType& vecIn ) const;
		bool equal ( const ThisType& vecIn, ValueType fuzz = TraitType::fuzzVal ) const;

		// empty methods
		void setEmpty ();
		bool isEmpty () const;

		// contains methods
		int contains ( ValueType xval, ValueType yval, ValueType zval ) const;
		int contains ( const vec3& vecIn  ) const;
		int contains ( const box3& boxIn  ) const;
		int contains ( const sphere& sphereIn  ) const;
		int contains ( const cylinder& cylIn  ) const;
		int contains ( const seg& segIn ) const;
		//int contains ( const frustum& frustIn ) const;
		//int contains ( const polytope& ptopeIn ) const;
		
		// extend by methods
		void extendBy ( ValueType xval, ValueType yval, ValueType zval );
		void extendBy ( const vec3& pt );
		void extendBy ( const sphere& sphere );
		void extendBy ( const box3& box );
		void extendBy ( const cylinder& cyl );
		//void extendBy ( const frustum& frustIn );
		//void extendBy ( const polytope& ptopeIn );

		// xform method
		void xformOrtho ( const cylinder& cyl, const matrix4& mat );
		void xformOrtho4 ( const cylinder& cyl, const matrix4& mat );

	protected:
		vec3		center;
		ValueType	radius;
		vec3		axis;
		ValueType	halfLength;

	private:

};

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnicylinder_h




