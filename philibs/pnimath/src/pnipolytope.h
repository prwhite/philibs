/////////////////////////////////////////////////////////////////////
//
//	class: polytope
//
/////////////////////////////////////////////////////////////////////


#ifndef pnipolytope_h
#define pnipolytope_h

/////////////////////////////////////////////////////////////////////


#include "pnimathdef.h"

#include "pnimath.h"

#include <vector>    // STD C++ Library

#ifdef _WIN32
#pragma warning(disable:4251)
#endif

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {


/////////////////////////////////////////////////////////////////////

class vec3;
class box3;
class sphere;
class cylinder;
class plane;
class matrix4;
class seg;

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI polytope 
{
	public:
		typedef Trait::ValueType ValueType;

		polytope ();
		polytope ( const polytope& orig );
		virtual ~polytope ();
		polytope& operator = ( const polytope& orig );


		// empty methods
		void setEmpty ();
		bool isEmpty () const;

	    // sets and gets
		int getNumPlanes () const;
		void addPlane ( const plane& planeIn );
		void addPlane ( ValueType x, ValueType y, ValueType z, ValueType d );
		void setPlane ( int idx, const plane& planeIn );
		void getPlane ( int idx, plane& planeOut ) const;

		// Containment of various object types.
	    int contains ( const vec3& pt ) const;
	    int contains ( const sphere& sphere ) const;
		int contains ( const box3& box ) const;
		int contains ( const cylinder& cyl ) const;
		int contains ( const seg& segIn ) const;
		//int contains ( const polytope& ptope ) const;

		// extendBy methods
		void extendBy ( const vec3& pt );
		void extendBy ( const box3& boxIn );
		void extendBy ( const sphere& sphereIn );
		void extendBy ( const seg& segIn );
		//void extendBy ( const cylinder& cylIn );
		//void extendBy ( const frustum& frustIn );
		//void extendBy ( const polytope& ptopeIn );


		// Xform interface.
		void xformOrtho ( const polytope& src, const matrix4& mat );
		void xformOrtho4 ( const polytope& src, const matrix4& mat );


	protected:
		typedef std::vector < plane* > planeList;
		planeList planes;
		
	private:
};

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end of namespace pni 

#endif // pnipolytope_h


