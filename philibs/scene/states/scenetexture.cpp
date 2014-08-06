/////////////////////////////////////////////////////////////////////
//
//    file: scenetexture.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenetexture.h"
#include "pnirefcountdbg.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

texture::texture() :
  mDirty ( DirtyAll ),
  mPriority ( 0.5f ),
  mAnisotropy ( 0.0f ),
  mTarget ( Tex2DTarget ),
  mMinFilter ( MinLinear ),
  mMagFilter ( MagLinear ),
  mSwrap ( Repeat ),
  mTwrap ( Repeat ),
  mRwrap( Repeat ),
  mSemantic(Diffuse)
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

bool texture::hasMipMaps () const
{
  return getNumImages() && ( getImage()->mBuffers.size() > 1 );
}
  
bool texture::needsGenMipMaps () const
{
  return ( mMinFilter >= MinNearestMipNearest ) && ( ! hasMipMaps() );
}

/////////////////////////////////////////////////////////////////////

void texture::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  state::collectRefs ( refs );
  
  pni::pstd::dbgRefCount::collectVecRefs ( mImgs, refs );
}

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 


