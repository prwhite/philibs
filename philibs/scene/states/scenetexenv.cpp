/////////////////////////////////////////////////////////////////////
//
//    file: scenetexenv.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenetexenv.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

texEnv::texEnv() :
  mMode ( Modulate ),
  mCombineRGB ( RGBModulate ),
  mCombineAlpha ( AModulate ),
  mBlendColor ( 0.0f, 0.0f, 0.0f, 0.0f )
{
  setRgbSourceType ( Arg0 );
  setRgbSourceType ( Arg1 );
  setRgbSourceType ( Arg2 );

  setAlphaSourceType ( Arg0 );
  setAlphaSourceType ( Arg1 );
  setAlphaSourceType ( Arg2 );

  setRgbOp ( Arg0 );
  setRgbOp ( Arg1 );
  setRgbOp ( Arg2 );

  setAlphaOp ( Arg0 );
  setAlphaOp ( Arg1 );
  setAlphaOp ( Arg2 );
}

texEnv::~texEnv()
{
  
}

// texEnv::texEnv(texEnv const& rhs)
// {
//   
// }
// 
// texEnv& texEnv::operator=(texEnv const& rhs)
// {
//   
//   return *this;
// }
// 
// bool texEnv::operator==(texEnv const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from state


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


