/////////////////////////////////////////////////////////////////////
//
//    file: scenelight.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenelight.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

light::light() :
  mType ( Directional ),
  mAmbient ( 0.2f, 0.2f, 0.2f, 1.0f ),
  mDiffuse ( 0.8f, 0.8f, 0.8f, 1.0f ),
  mSpecular ( 1.0f, 1.0f, 1.0f, 1.0f ),
  mPosition ( 0.0f, 0.0f, 0.0f, 0.0f ),
  mDirection ( 0.0f, 1.0f, 0.0f ),
  mExponent ( 1.0f ),
  mCutoffAngle ( 180.0f ),
  mAttenuation ( 1.0f, 0.0f, 0.0f )
{
  
}

// light::~light()
// {
//   
// }
// 
// light::light(light const& rhs)
// {
//   
// }
// 
// light& light::operator=(light const& rhs)
// {
//   
//   return *this;
// }
// 
// bool light::operator==(light const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from node


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


