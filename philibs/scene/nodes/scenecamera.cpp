/////////////////////////////////////////////////////////////////////
//
//    file: scenecamera.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenecamera.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

camera::camera() :
  mClearMask ( Color | Depth ),
  mColorClear ( pni::math::vec4 () ),
  mAccumClear ( pni::math::vec4 () ),
  mStencilClear ( 0x00 ),
  mDepthClear ( 1.0f ),
  mNormalizeMode ( NoNormalize ),
  mEnableScissor ( false )
{
  setSymmetric ();
  setViewport ();
}

camera::~camera()
{
  
}

// camera::camera(camera const& rhs) :
// //   node ( rhs )
// {
//   
// }

// camera& camera::operator=(camera const& rhs)
// {
//   
//   return *this;
// }
// 
// bool camera::operator==(camera const& rhs) const
// {
//   
//   return false;
// }


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// View dependent conversions.
void camera::screenToWorld ( pni::math::vec3& dst, const pni::math::vec3& src )
{
  using namespace pni;
  
  math::matrix4 tmp ( calcViewportMatrix () );
  tmp.preMult ( calcProjectionMatrix () );
  tmp.invert ();
  
  dst.xformPt4 ( src, tmp );
}

void camera::worldToScreen ( pni::math::vec3& dst, const pni::math::vec3& src )
{
  using namespace pni;
  
  math::matrix4 tmp ( calcViewportMatrix () );
  tmp.preMult ( calcProjectionMatrix () );
  
  dst.xformPt4 ( src, tmp );
}

/////////////////////////////////////////////////////////////////////

void camera::getFrustum ( pni::math::frustum& frust ) const
{
	if ( mProjType == Ortho )
	{
		frust.setType ( pni::math::frustum::Ortho );
		frust.set ( mNear, mFar, mLeft, mRight, mBottom, mTop );
	}
	else	// perspective
	{
		frust.setType ( pni::math::frustum::Perspective );
		
		if ( mPerspType == Symmetric )
		{
			// calc left, right, top and bottom from hfov, vfov
			
			float bt = mNear * pni::math::Trait::tan ( pni::math::Trait::d2r ( mVfov ) * ( ( float ) 0.5 ) );
			float lr = mNear * pni::math::Trait::tan ( pni::math::Trait::d2r ( mHfov ) * ( ( float ) 0.5 ) );
	
			frust.set ( mNear, mFar, -lr, lr, -bt, bt );
		}
		else	// asymmetric
		{
			frust.set ( mNear, mFar, mLeft, mRight, mBottom, mTop );
		}
	}
}

/////////////////////////////////////////////////////////////////
// helper methods
pni::math::matrix4 const& camera::calcProjectionMatrix ()
{
	// UNTESTED

	if ( mProjType == Ortho )
	{
		mProjMat.setOrtho ( mLeft, mRight, mBottom, mTop, mNear, mFar );
	}
	else
	{
		if ( mPerspType == Symmetric )
		{
			float vpAspect = mVpWidth / mVpHeight ;

			if ( mHfov < 0.0f ) mHfov = vpAspect * mVfov;
			if ( mVfov < 0.0f ) mVfov = mHfov / vpAspect;

			mProjMat.setPerspective ( mVfov, mHfov / mVfov, mNear, mFar );
		}
		else
		{
			mProjMat.setFrustum ( mLeft, mRight, mBottom, mTop, mNear, mFar );
		}
	}
  
  return mProjMat;
}

pni::math::matrix4 const& camera::calcViewportMatrix ()
{
  mVpMat.setViewport ( mVpLeft, mVpBottom, mVpWidth, mVpHeight );
  return mVpMat;
}


/////////////////////////////////////////////////////////////////////
// overrides from node


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


