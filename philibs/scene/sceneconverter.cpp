/////////////////////////////////////////////////////////////////////
//
//    file: sceneconverter.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneconverter.h"

#include "scenecamera.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

converter::converter() :
  mCam ( 0 )
{
  
}

// converter::~converter()
// {
//   
// }
// 
// converter::converter(converter const& rhs)
// {
//   
// }
// 
// converter& converter::operator=(converter const& rhs)
// {
//   
//   return *this;
// }
// 
// bool converter::operator==(converter const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void converter::getConvertMatrix ( pni::math::matrix4& dst,
    nodePath const& dstPath, nodePath const& srcPath )
{
  using namespace pni;

  math::matrix4 srcMat ( math::matrix4::NoInit );
  srcPath.calcXform ( srcMat );
  
  math::matrix4 dstMat ( math::matrix4::NoInit );
  dstPath.calcInverseXform ( dstMat );
  
  dstMat.preMult ( srcMat );
  dst = dstMat;
}

/////////////////////////////////////////////////////////////////////

// dstPath must be to a camera node.
bool converter::getWorldToScreenMatrix ( pni::math::matrix4& dstMat,
    nodePath const& dstPath, nodePath const& srcPath )
{
  using namespace pni;

  mCam = 0;
  dstPath.getLeaf ()->accept ( this );
  
  if ( mCam )
  {
    dstMat = mCam->getViewportMatrix ();
    dstMat.preMult ( mCam->getProjectionMatrix () );

    math::matrix4 convMat ( math::matrix4::NoInit );
    getConvertMatrix ( convMat, dstPath, srcPath );
    
    dstMat.preMult ( convMat );
    return true;
  }
  else
    return false;
}

/////////////////////////////////////////////////////////////////////

bool converter::getWorldToCamMatrix  ( pni::math::matrix4& dstMat,
    nodePath const& dstPath, nodePath const& srcPath )
{
  using namespace pni;

  mCam = 0;
  dstPath.getLeaf ()->accept ( this );
  
  if ( mCam )
  {
    dstMat = mCam->getProjectionMatrix ();

    math::matrix4 convMat ( math::matrix4::NoInit );
    getConvertMatrix ( convMat, dstPath, srcPath );
    
    dstMat.preMult ( convMat );
    return true;
  }
  else
    return false;
}

/////////////////////////////////////////////////////////////////////

void converter::convert ( pni::math::vec3& dst, pni::math::vec3 const& src,
    nodePath const& dstPath, nodePath const& srcPath )
{
  using namespace pni;
  
  math::matrix4 dstMat ( math::matrix4::NoInit );
  getConvertMatrix ( dstMat, dstPath, srcPath );
  dst.xformPt4 ( src, dstMat );
}

/////////////////////////////////////////////////////////////////////

// Note that dstPath should be a path to a camera node.
void converter::worldToScreen ( pni::math::vec3& dst, pni::math::vec3 const& src,
    nodePath const& dstPath, nodePath const& srcPath )
{
  using namespace pni;
  
  math::matrix4 mat ( math::matrix4::NoInit );
  getWorldToScreenMatrix ( mat, dstPath, srcPath );
  
  dst.xformPt4 ( src, mat );
}

/////////////////////////////////////////////////////////////////////

// Note that srcPath should be a path to a camera node.
void converter::screenToWorld ( pni::math::vec3& dst, pni::math::vec3 const& src,
    nodePath const& dstPath, nodePath const& srcPath )
{
  using namespace pni;
  
  math::matrix4 mat ( math::matrix4::NoInit );
  getWorldToScreenMatrix ( mat, srcPath, dstPath );
  mat.invert ();
  
  dst.xformPt4 ( src, mat );
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// Using double dispatch instead of dynamic cast.
void converter::dispatch ( camera const* pNode )
{
  mCam = pNode;
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


