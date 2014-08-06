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
  setViewport ();
  setSymmetric ();
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

namespace // anonymous
{

float calcMissingFov ( float angle, float hvp, float vvp )
{
  using namespace pni::math;

  float d = hvp / Trait::tan ( Trait::d2r ( angle * 0.5f ) );
  float ret = Trait::r2d ( Trait::atan2 ( vvp, d ) );
  return ret * 2.0f;
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
			if ( mHfov < 0.0f ) mHfov = calcMissingFov(mVfov, mVpHeight, mVpWidth);
			if ( mVfov < 0.0f ) mVfov = calcMissingFov(mHfov, mVpWidth, mVpHeight);

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


