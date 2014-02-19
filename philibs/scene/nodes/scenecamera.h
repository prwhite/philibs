/////////////////////////////////////////////////////////////////////
//
//    class: camera
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenecamera_h
#define scenescenecamera_h

/////////////////////////////////////////////////////////////////////

#include "scenenode.h"
#include "scenegraphdd.h"

#include "pnimatrix4.h"
#include "pnivec3.h"
#include "pnivec4.h"
#include "pnifrustum.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

  /**
    Camera node.  Instances of this class are part of the scene graph,
    so they will inherit transformations, etc.  But, they are used by
    specifiying a #nodePath for the camera to the #graphDd that is rendering
    the scene graph.  This way, the grappDd unambigously knows what camera
    is active for rendering.
    @note Generally, defaults for this class are as needed for basic rendering,
    but the projection and viewport should be set with applicaiton values.
    @note With >= GLES 2.0, the normalize mode will be caried out through
    an optimization in the ddOgl that will only create a special normal matrix
    when the mode is not NoNormalize _and_ a node's transformation is not
    identity.
  */
class camera :
  public node
{
  public:
    camera();
    virtual ~camera();
//     camera(camera const& rhs);
//     camera& operator=(camera const& rhs);
//     bool operator==(camera const& rhs) const;


    ////////////////////////////////////////////////////////////
		// Buffer clear.
		enum Buffer { 
				BufferNone = 0, 
				Color = 1, 
				Depth = 2, 
				Stencil = 4, 
				Accum = 8 
		};	// Stereo buffers?  Other buffers?
		
		void setClearMask ( unsigned int maskIn = Color | Depth ) { mClearMask = maskIn; }
		unsigned int getClearMask () const { return mClearMask; }
		
		// color buffer
		void setColorClear ( pni::math::vec4 colorIn = pni::math::vec4 ( 0.0f, 0.0f, 0.0f, 0.0f ) ) { mColorClear = colorIn; }
		pni::math::vec4 const& getColorClear () const { return mColorClear; }

		// accum buffer
		void setAccumClear ( pni::math::vec4 accumIn = pni::math::vec4 ( 0.0f, 0.0f, 0.0f, 0.0f ) ) { mAccumClear = accumIn; }
		pni::math::vec4 const& getAccumClear () const { return mAccumClear; }

		// stencil
		void setStencilClear ( unsigned int valIn = 0x00 ) { mStencilClear = valIn; }
		int getStencilClear () const { return mStencilClear; }

		// depth
		void setDepthClear ( float valIn = 1.0f ) { mDepthClear = valIn; }
		float getDepthClear () const { return mDepthClear; }


    ////////////////////////////////////////////////////////////
    // Normal normalization.
		enum NormalizeMode { NoNormalize, Normalize, Rescale };
		void setNormalizeMode ( NormalizeMode typeIn = NoNormalize ) { mNormalizeMode = typeIn; }
		NormalizeMode getNormalizeMode () const { return mNormalizeMode; }


    ////////////////////////////////////////////////////////////
		// projection type
		enum ProjectionType { Ortho, Perspective };		// Perspective is default
		void setType ( ProjectionType typeIn ) { mProjType = typeIn; }
		ProjectionType getType () const { return mProjType; }

		// perspective projection type
		enum PerspectiveType { Symmetric, Asymmetric };	// Symmetric is default
		void setPerspectiveType ( PerspectiveType typeIn ) { mPerspType = typeIn; }
		PerspectiveType getPerspectiveType () const { return mPerspType; }


    ////////////////////////////////////////////////////////////
		// frustum specifications
		void setOrtho ( float nearIn, float farIn, float leftIn, float rightIn, float bottomIn, float topIn )
				{
					setType ( Ortho );
					mNear = nearIn; mFar = farIn; mLeft = leftIn; mRight = rightIn; mBottom = bottomIn; mTop = topIn;
					calcProjectionMatrix ();
				}
		void getOrtho ( float& nearIn, float& farIn, float& leftIn, float& rightIn, float& bottomIn, float& topIn ) const
				{
					nearIn = mNear; farIn = mFar; leftIn = mLeft; rightIn = mRight; bottomIn = mBottom; topIn = mTop;
				}

		void setAsymmetric ( float nearIn, float farIn, float leftIn, float rightIn, float bottomIn, float topIn )
				{
					setType ( Perspective );
					setPerspectiveType ( Asymmetric );
          mNear = nearIn; mFar = farIn; mLeft = leftIn; mRight = rightIn; mBottom = bottomIn; mTop = topIn;
					calcProjectionMatrix ();
				}
		void getAsymmetric ( float& nearIn, float& farIn, float& leftIn, float& rightIn, float& bottomIn, float& topIn ) const
				{
					nearIn = mNear; farIn = mFar; leftIn = mLeft; rightIn = mRight; bottomIn = mBottom; topIn = mTop;
				}

		// -1 to hfovIn or vfovIn will cause the other to be calculated and stored for a 1:1 aspect ratio
		void setSymmetric ( float nearIn = 0.1f, float farIn = 1000.0f, float hfovIn = 90.0f, float vfovIn = -1.0f )
				{
					setType ( Perspective );
					setPerspectiveType ( Symmetric );
					mNear = nearIn; mFar = farIn; mHfov = hfovIn; mVfov = vfovIn;
					calcProjectionMatrix ();
				}

		void getSymmetric ( float& nearIn, float& farIn, float& hfovIn, float& vfovIn ) const
				{
          nearIn = mNear; farIn = mFar; hfovIn = mHfov; vfovIn = mVfov;
				}

		// set/get viewport.  The defaults set up a 4to3 aspect ratio for fov calcs. YMMV
		// The window origin is in the bottom left.
		void setViewport ( float vpLeftIn = 0.0f, float vpBottomIn = 0.0f, float vpWidthIn = 4.0f, float vpHeightIn = 3.0f )
				{
					mVpLeft = vpLeftIn; mVpBottom = vpBottomIn; mVpWidth = vpWidthIn; mVpHeight = vpHeightIn;
					calcViewportMatrix ();
				}

		void getViewport ( float& vpLeftOut, float& vpBottomOut, float& vpWidthOut, float& vpHeightOut ) const
				{
					vpLeftOut = mVpLeft; vpBottomOut = mVpBottom; vpWidthOut = mVpWidth; vpHeightOut = mVpHeight;
				}

		void getFrustum ( pni::math::frustum& frust ) const;


    /////////////////////////////////////////////////////////////////
		// scissor to viewport or not
		void setEnableScissor ( bool valIn = false ) { mEnableScissor = valIn; }
		bool getEnableScissor () const { return mEnableScissor; }


    /////////////////////////////////////////////////////////////////
		// View dependent conversions.
		void screenToWorld ( pni::math::vec3& dst, const pni::math::vec3& src );
		void worldToScreen ( pni::math::vec3& dst, const pni::math::vec3& src );

		
    /////////////////////////////////////////////////////////////////
    // helper methods
		pni::math::matrix4 const& calcProjectionMatrix ();	// call this when other values change
		pni::math::matrix4 const& getProjectionMatrix () const { return mProjMat; }

		pni::math::matrix4 const& calcViewportMatrix ();
		pni::math::matrix4 const& getViewportMatrix () const { return mVpMat; }


  protected:
    
  private:

		unsigned int mClearMask;
		pni::math::vec4 mColorClear;
		pni::math::vec4 mAccumClear;
		unsigned int mStencilClear;
		float mDepthClear;
    
    NormalizeMode mNormalizeMode;
    
    ProjectionType mProjType;
    PerspectiveType mPerspType;
    
    float mNear;
    float mFar;
    float mLeft;
    float mRight;
    float mBottom;
    float mTop;
    float mHfov;
    float mVfov;
    pni::math::matrix4 mProjMat;
    
    float mVpLeft;
    float mVpBottom;
    float mVpWidth;
    float mVpHeight;
    pni::math::matrix4 mVpMat;
    
    bool mEnableScissor;

  // interface from node
  public:
    virtual node* dup () const { return new camera ( *this ); }
    virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }

  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenecamera_h


