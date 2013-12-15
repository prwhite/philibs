/////////////////////////////////////////////////////////////////////
//
//	class: matStack
//
//	This class is templatized on a matrix type to create a fixed-
//	size matrix stack.
//	Note:  This header does not include it's implementation file
//	like many template classes do.  In order to instantiate a
//	version of this template, clients must manually include the
//	cpp file as well.  This pni math library already includes an
//	instantiation for the matrix4 type.
//
/////////////////////////////////////////////////////////////////////


#ifndef pnimatstack_h
#define pnimatstack_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

template <class MatType>
class matStack { 
	public:
		enum InitState { NoInit };
		enum StackVals { DefaultDepth = 32, MaxDepth = 128 };
	
		matStack ( int depthIn = DefaultDepth );
		matStack ( InitState, int depthIn = DefaultDepth );
		~matStack ();
		
		void reset ();
		int getMaxDepth () const { return depth; }
		int getCurDepth () const { return cur; }
		
		bool push ();
		bool pop ();
		
		MatType* operator -> ();
		const MatType* operator -> () const;
		MatType& operator * ();
		const MatType& operator * () const;
		
	protected:
	
	private:
		
		class PrivMat :
			public MatType
		{
			public:
				PrivMat () : MatType ( MatType::NoInit ) {}
		};

		PrivMat* stack;
		int depth;
		int cur;
	
};

/////////////////////////////////////////////////////////////////////

template <class MatType>
PNIMATHINLINE
MatType*
matStack<MatType>::
operator -> ()
{
	return &stack[ cur ];
}

template <class MatType>
PNIMATHINLINE
const MatType*
matStack<MatType>::
operator -> () const
{
	return &stack[ cur ];
}

template <class MatType>
PNIMATHINLINE
MatType&
matStack<MatType>::
operator * ()
{
	return stack[ cur ];
}

template <class MatType>
PNIMATHINLINE
const MatType&
matStack<MatType>::
operator * () const
{
	return stack[ cur ];
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnimatstack_h

