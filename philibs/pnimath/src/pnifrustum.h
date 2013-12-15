/////////////////////////////////////////////////////////////////////
//
//	class: frustum
//
/////////////////////////////////////////////////////////////////////


#ifndef pnifrustum_h
#define pnifrustum_h

/////////////////////////////////////////////////////////////////////

#include "pnimathdef.h"
#include "pnimath.h"
#include "pniplane.h"
#include "pnimatrix4.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {
	
/////////////////////////////////////////////////////////////////////

class PNIMATHAPI frustum
{
	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef frustum ThisType;

		enum InitState { NoInit };
		enum PlaneIndex
		{
			NearPlane = 0,
			FarPlane,
			LeftPlane,
			RightPlane,
			BottomPlane,
			TopPlane
		};

		frustum ();
		frustum ( InitState );

		// Makes a perspective frustum
		frustum ( ValueType nearIn, ValueType farIn,
				  ValueType leftIn, ValueType rightIn,
				  ValueType bottomIn, ValueType topIn );
		frustum ( const frustum& orig );
		virtual ~frustum ();
		frustum& operator = ( const frustum& orig );


		// set/get
		void set ( ValueType nearIn, ValueType farIn,
				  ValueType leftIn, ValueType rightIn,
				  ValueType bottomIn, ValueType topIn );
		void get ( ValueType& nearOut, ValueType& farOut,
				  ValueType& leftOut, ValueType& rightOut,
				  ValueType& bottomOut, ValueType& topOut ) const;

		// Projection related methods.
		// Defaults to Perspective
		enum Type { Ortho, Perspective };
		void setType ( Type typeIn ) { type = typeIn; recompute (); }
		Type getType () const { return type; }

	    // Near and far clipping planes.
		ValueType getNearDistance () const;
		void setNearDistance ( ValueType nearIn );

		ValueType getFarDistance () const;
		void setFarDistance ( ValueType farIn );

		// Left/right/top/bottom planes.
		ValueType getLeft () const;
		void setLeft ( ValueType valIn );
		 
		ValueType getRight () const;
		void setRight ( ValueType valIn );

		ValueType getTop () const;
		void setTop ( ValueType valIn );

		ValueType getBottom () const;
		void setBottom ( ValueType valIn );

		void getPlane ( plane& dst, PlaneIndex which );

		// Get the 8 points that make up the frustum.
		// Only valid if the frustum is unxformed and 
		// unextended.
		enum Extents {
			NearBottomLeft,
			NearBottomRight,
			NearTopLeft,
			NearTopRight,
			FarBottomLeft,
			FarBottomRight,
			FarTopLeft,
			FarTopRight
		};
		void getExtents ( vec3 points[ 8 ] );

		// Xform interface.
		//void getMatrix ( matrix4& matrixOut ) const;
		void xformOrtho ( const frustum& src, const matrix4& mat );
		void xformOrtho4 ( const frustum& src, const matrix4& mat );

		// Containment of various object types.
	    int contains ( const vec3& pt ) const;
	    int contains ( const sphere& sphere ) const;
		int contains ( const box3& box ) const;
		int contains ( const cylinder& cyl ) const;
		int contains ( const seg& segIn ) const;
		int contains ( const frustum& frustIn ) const;

		// extendBy()
			//mtcl: mtcl_begin_ignore
		void extendBy ( ValueType xval, ValueType yval, ValueType zval );
		void extendBy ( const vec3& pt );
		void extendBy ( const sphere& sphereIn );
		void extendBy ( const box3& boxIn );
		void extendBy ( const seg& segIn );
		//void extendBy ( const cylinder& cylIn );
		//void extendBy ( const frustum& frustIn );
		//void extendBy ( const polytope& ptopeIn );
			//mtcl: mtcl_end_ignore


	protected:


	private:
		plane planes[6];          // See enumeration above for indexing.
		ValueType nearDistance;
		ValueType farDistance;
		ValueType left;
		ValueType right;
		ValueType bottom;
		ValueType top;
		//matrix4 xformMatrix;	  // Transformation matrix.
		
		Type type;

		void recompute ();
};

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end of namespace pni 

#endif // pnifrustum_h


