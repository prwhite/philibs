/////////////////////////////////////////////////////////////////////
//
//	file: pnipolytope.cpp
//
/////////////////////////////////////////////////////////////////////

#include "pnipolytope.h"

#include "pniplane.h"
#include "pnivec3.h"
#include "pnisphere.h"
#include "pnicylinder.h"
#include "pnibox3.h"

#include <cassert>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {
	
/////////////////////////////////////////////////////////////////////

polytope::
polytope ()
{
	
}



polytope::
polytope ( const polytope& orig )
{
	planeList::const_iterator i = orig.planes.begin ();

	while ( i < orig.planes.end () )
	{
		planes.push_back ( new plane ( **i ) );
		++i;
	}
}


polytope::
~polytope ()
{
	setEmpty ();
}




polytope&
polytope::
operator = ( const polytope& orig )
{
	setEmpty ();

	planeList::const_iterator i = orig.planes.begin ();

	while ( i < orig.planes.end () )
	{
		planes.push_back ( new plane ( **i ) );
		++i;
	}
	
	return *this;
}



// empty methods
void
polytope::
setEmpty ()
{
	planeList::iterator i = planes.begin ();

	while ( i < planes.end () )
	{
		delete *i;
		planes.erase ( i );
		++i;
	}
}


bool
polytope::
isEmpty () const
{
	return planes.size () == 0;
}




// sets and gets
size_t
polytope::
getNumPlanes () const
{
	return planes.size ();
}



void
polytope::
addPlane ( const plane& planeIn )
{
	planes.push_back ( new plane ( planeIn ) );	
}




void
polytope::
addPlane ( ValueType x, ValueType y, ValueType z, ValueType d )
{
	planes.push_back ( new plane ( x, y, z, d ) );
}






void
polytope::
setPlane ( size_t idx, const plane& planeIn )
{
	assert ( idx < planes.size () );

	if ( planes [ idx ] )
	{
		delete planes[ idx ];
	}

	planes[ idx ] = new plane ( planeIn );
}



//plane getPlane ( int idx ) const;
//const plane* getPlane ( int idx ) const;

void
polytope::
getPlane ( size_t idx, plane& planeOut ) const
{
	assert ( idx < planes.size () );

	planeOut = *planes[ idx ];
}


// Xform interface.
void
polytope::
xformOrtho ( const polytope& src, const matrix4& mat )
{
	planeList::iterator i = planes.begin ();

	while ( i < planes.end () )
	{
		( **i ).xformOrtho ( **i, mat );
		++i;
	}
}


// orthoXform4 is for a homogeneous/uniform 4x4 matrix.
void
polytope::
xformOrtho4 ( const polytope& src, const matrix4& mat )
{
	planeList::iterator i = planes.begin ();

	while ( i < planes.end () )
	{
		( **i ).xformOrtho4 ( **i, mat );
		++i;
	}
}



// extendBy methods
void
polytope::
extendBy ( const vec3& pt )
{
	planeList::iterator i = planes.begin ();

	while ( i < planes.end () )
	{
		( **i ).extendBy ( pt );
		++i;
	}
}



void
polytope::
extendBy ( const box3& boxIn )
{
	planeList::iterator i = planes.begin ();

	while ( i < planes.end () )
	{
		( **i ).extendBy ( boxIn );
		++i;
	}
}



void
polytope::
extendBy ( const sphere& sphereIn )
{
	planeList::iterator i = planes.begin ();

	while ( i < planes.end () )
	{
		( **i ).extendBy ( sphereIn );
		++i;
	}
}




void
polytope::
extendBy ( const seg& segIn )
{
	planeList::iterator i = planes.begin ();

	while ( i < planes.end () )
	{
		( **i ).extendBy ( segIn );
		++i;
	}
}






// Containment of various types.
int
polytope::
contains ( const vec3& ptIn ) const
{
	planeList::const_iterator i = planes.begin ();
	int contains = containsResult::SomeIn | containsResult::AllIn;

	while ( i < planes.end () )
	{
		int c = ( *i )->contains ( ptIn );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
		++i;
	}

	return contains;
}


int
polytope::
contains ( const sphere& sphereIn ) const
{
	planeList::const_iterator i = planes.begin ();
	int contains = containsResult::SomeIn | containsResult::AllIn;

	while ( i < planes.end () )
	{
		int c = ( *i )->contains ( sphereIn );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
		++i;
	}

	return contains;
}


int
polytope::
contains ( const box3& boxIn ) const
{
	planeList::const_iterator i = planes.begin ();
	int contains = containsResult::SomeIn | containsResult::AllIn;

	while ( i < planes.end () )
	{
		int c = ( *i )->contains ( boxIn );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
		++i;
	}

	return contains;
}



int
polytope::
contains ( const cylinder& cyl ) const
{
	// TODO: pni::plane doesn't implement the cylinder contains() method.
	// Once pni:plane implements contains () for a cylinder, this should work...
	PNIMATHUNIMPLEMENTED;

	planeList::const_iterator i = planes.begin ();
	int contains = containsResult::SomeIn | containsResult::AllIn;

	while ( i < planes.end () )
	{
		int c = ( *i )->contains ( cyl );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
		++i;
	}

	return contains;
}






#if 0
int
polytope::
contains ( const polytope& ptope ) const
{
	planeList::const_iterator i = planes.begin ();
	//plane::Contains contains = containsResult::NoneIn;
	int contains = containsResult::SomeIn | containsResult::AllIn;

	while ( i < planes.end () )
	{
		planeList::const_iterator j = ptope.planes.begin ();

		while ( j < ptope.planes.end () )
		{
			//plane::Contains c = static_cast< plane::Contains > ( ( *i )->contains ( *j ) );
			int c = ( *i )->contains ( *j );

			if ( c == containsResult::NoneIn )
			{
				return containsResult::NoneIn;
			}

			contains &= c;
			++j;
		}

		++i;
	}

	return contains;
}
#endif



int
polytope::
contains ( const seg& segIn ) const
{
	planeList::const_iterator i = planes.begin ();
	//plane::Contains contains = containsResult::NoneIn;
	int contains = containsResult::SomeIn | containsResult::AllIn;

	while ( i < planes.end () )
	{
		//plane::Contains c = static_cast< plane::Contains > ( ( *i )->contains ( segIn ) );
		int c = ( *i )->contains ( segIn );

		if ( c == containsResult::NoneIn )
		{
			return containsResult::NoneIn;
		}

		contains &= c;
		++i;
	}

	return contains;
}


/////////////////////////////////////////////////////////////////////


	} // end namespace math
} // end of namespace pni 


