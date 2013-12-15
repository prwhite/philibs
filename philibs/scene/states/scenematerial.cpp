/////////////////////////////////////////////////////////////////////
//
//    file: scenematerial.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenematerial.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

material::material() :
    mDiffuse ( 1.0f, 1.0f, 1.0f, 1.0f ),  // Not OGLES default.
    mAmbient ( 0.8f, 0.8f, 0.8f, 1.0f ),
    mSpecular ( 1.0f, 1.0f, 1.0f, 1.0f ),
    mEmissive ( 0.0f, 0.0f, 0.0f, 1.0f ),
    mShininess ( 20.0f ),
//     mCmode ( ModeNone ),
    mColorMaterialMode ( false )
{
  
}

material::~material()
{
  
}

// material::material(material const& rhs)
// {
//   
// }
// 
// material& material::operator=(material const& rhs)
// {
//   
//   return *this;
// }
// 
// bool material::operator==(material const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from state


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


