/////////////////////////////////////////////////////////////////////
//
//    file: scenetexgen.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenetexgen.h"

/////////////////////////////////////////////////////////////////////

namespace scene {

#include "pnivec4.h"
#include "pnimatrix4.h"
  
/////////////////////////////////////////////////////////////////////

texGen::texGen()
{
	setFunction ( SCoord );
	setFunction ( TCoord );
	setFunction ( RCoord );
	setFunction ( QCoord );

	setPlane ( SCoord );
	setPlane ( TCoord );
	setPlane ( RCoord );
	setPlane ( QCoord );
}

texGen::~texGen()
{
  
}

// texGen::texGen(texGen const& rhs)
// {
//   
// }
// 
// texGen& texGen::operator=(texGen const& rhs)
// {
//   
//   return *this;
// }
// 
// bool texGen::operator==(texGen const& rhs) const
// {
//   
//   return false;
// }

inline
void
setRowAsPlane ( texGen::Coord coord, const pni::math::matrix4& mat4, texGen* tgen )
{
  using namespace pni;
  
	math::vec4 row ( math::vec4::NoInit );
	math::plane pln ( math::plane::NoInit );

	mat4.getRow ( coord, row );	
	pln.set ( row[ 0 ], row[ 1 ], row[ 2 ], row[ 3 ] );

	tgen->setPlane ( coord, pln );
}

void texGen::setMatrix ( const pni::math::matrix4& mat4, Function funcIn )
{
	if ( funcIn != ObjectLinear && funcIn != EyeLinear ) return;

	setFunction ( SCoord, funcIn );
	setFunction ( TCoord, funcIn );
	setFunction ( RCoord, funcIn );
	setFunction ( QCoord, funcIn );

	setRowAsPlane ( SCoord, mat4, this );
	setRowAsPlane ( TCoord, mat4, this );
	setRowAsPlane ( RCoord, mat4, this );
	setRowAsPlane ( QCoord, mat4, this );
}


/////////////////////////////////////////////////////////////////////
// overrides from state


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


