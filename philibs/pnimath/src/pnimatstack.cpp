/////////////////////////////////////////////////////////////////////
//
//	matStack.C
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

#include "pnimatstack.h"
#include <cassert>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

template <class MatType>
matStack<MatType>::
matStack ( int depthIn ) :
	depth ( depthIn ), 
	cur ( 0 )
{
	stack = new PrivMat[ depth ];
	stack[ 0 ].setIdentity ();
}

template <class MatType>
matStack<MatType>::
matStack ( InitState, int depthIn ) :
	depth ( depthIn ), 
	cur ( 0 )
{
	stack = new PrivMat[ depth ];
}

template <class MatType>
matStack<MatType>::
~matStack ()
{
	delete[] stack;
}

template <class MatType>
void
matStack<MatType>::
reset ()
{
	cur = 0;
}

/////////////////////////////////////////////////////////////////////

template <class MatType>
bool
matStack<MatType>::
push ()
{
	assert ( cur + 1 < depth );
		
	stack[ cur + 1 ] = stack[ cur ];
	cur++;
	
	return true;
}

template <class MatType>
bool
matStack<MatType>::
pop ()
{
	assert ( cur > 0 );
	
	cur--;

	return true;
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni


