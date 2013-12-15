/////////////////////////////////////////////////////////////////////
//
//    file: scenetexture.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenetexture.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

texture::texture() :
  mDirty ( DirtyTrue ),
  mPriority ( 0.5f ),
//   mAnisotropy ( 0.0f ),
  mTarget ( Tex2D ),
  mMinFilter ( MinLinear ),
  mMagFilter ( MagLinear ),
  mSwrap ( Repeat ),
  mTwrap ( Repeat )
//   mResize ( ResizeNone )
{
  
}

texture::~texture()
{
  
}

// texture::texture(texture const& rhs)
// {
//   
// }
// 
// texture& texture::operator=(texture const& rhs)
// {
//   
//   return *this;
// }
// 
// bool texture::operator==(texture const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////

void texture::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  state::collectRefs ( refs );
  
  refs.push_back ( mImg.get () );
}

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 


