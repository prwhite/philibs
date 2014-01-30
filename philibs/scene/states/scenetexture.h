/////////////////////////////////////////////////////////////////////
//
//    class: texture
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenetexture_h
#define scenescenetexture_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"
#include "imgbase.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class texture :
  public state
{
  public:
    texture();
    virtual ~texture();
//     texture(texture const& rhs);
//     texture& operator=(texture const& rhs);
//     bool operator==(texture const& rhs) const;

		enum Target {
      Tex2DTarget = 1,      // Numbers here are overloaded... used for resizing
      CubeMapTarget = 6,    // image array.
    };
    enum ImageId {
      Tex2D = 0,
      CubePosX = 0, // TRICKY: Tex2D and CubePosX are co-located
      CubeNegX,
      CubePosY,
      CubeNegY,
      CubePosZ,
      CubeNegZ,
      NumCubeSlots = 6
    };
		enum Dirty { DirtyFalse, DirtyTrue };
		enum MinFilter { 
      MinNearest, 
      MinLinear, 
      MinNearestMipNearest,   // mipmapped entries must be > non mipmapped entries
      MinLinearMipNearest,    // this convention is leveraged in scenetexobj.cpp
      MinNearestMipLinear, 
      MinLinearMipLinear 
    };
		enum MagFilter { MagNearest, MagLinear };

		enum Wrap {
      Repeat,
      ClampToEdge,
      RepeatMirrored
    };
// 		enum Resize { ResizeNone, Fill, Smaller, Bigger };
    enum Semantic {
      Diffuse,
      Specular,
      Ambient,
      Emissive,
      Height,
      Normals,
      Shininess,
      Opacity,
      Displacement,
      LightMap,
      Reflection,
      Depth,
      Unknown
    };

      /// Set texture to either 2D image or Cube Map via #Target enum.
		void setTarget ( Target targetIn = Tex2DTarget )
      {
        setDirty ( DirtyTrue );
        mImgs.resize ( targetIn );
        mTarget = targetIn;
        if ( mTarget == CubeMapTarget )
          setWrap ( ClampToEdge );
      }
  
		Target getTarget () const { return mTarget; }

      // texture sources
      /// Instances can store 0, 1 or 6 images, corresponding to real-time (maybe?),
      /// Tex2D or cube map target types.
		void setImage ( img::base* pImg, ImageId which = Tex2D )
      {
        setDirty ( DirtyTrue );
        setTarget ( mTarget );  // Just in case it hasn't been set.
        if ( which < mImgs.size () )
          mImgs[ which ] = pImg;
        else
          PNIDBGSTR("texture image out of range for target type");
      }
  
		img::base* getImage ( ImageId which = Tex2D ) const { return mImgs[ which ].get (); }
    size_t getNumImages () const { return mImgs.size (); }
    void resetImages () { setDirty ( DirtyTrue ); mImgs.resize ( 0 ); }

		// use this when texture parameters/data changes
    // TRICKY: setDirty is const... clearing dirty occurs for const
    // objects... blah blah blah.
		void setDirty ( Dirty dirtyIn = DirtyTrue )const { mDirty = dirtyIn; }
		Dirty getDirty () const { return mDirty; }

		void setMinFilter ( MinFilter minFilterIn = MinLinear ) { setDirty ( DirtyTrue ); mMinFilter = minFilterIn; }
		MinFilter getMinFilter () const { return mMinFilter; }

		void setMagFilter ( MagFilter magFilterIn = MagLinear ) { setDirty ( DirtyTrue ); mMagFilter = magFilterIn; }
		MagFilter getMagFilter () const { return mMagFilter; }

 		void setMaxAnisotropy ( float val = 0.0f ) { mAnisotropy = val; } // [0,1] = [Default,HWMax]
 		float getMaxAnisotropy () const { return mAnisotropy; }

		void setSWrap ( Wrap wrapIn = Repeat ) { setDirty ( DirtyTrue ); mSwrap = wrapIn; }
		Wrap getSWrap () const { return mSwrap; }

		void setTWrap ( Wrap wrapIn = Repeat ) { setDirty ( DirtyTrue ); mTwrap = wrapIn; }
		Wrap getTWrap () const { return mTwrap; }

      /// GLES 3.0 only.
      /// @note We are currently only building for GLES 2.0, so this interface
      /// is for future functionality.
		void setRWrap ( Wrap wrapIn = Repeat ) { setDirty ( DirtyTrue ); mRwrap = wrapIn; }
		Wrap getRWrap () const { return mRwrap; }
  
      /// Convenience method to set all wrap parameters at once to unique values.
    void setWrap ( Wrap swrap, Wrap twrap, Wrap rwrap )
      { setDirty ( DirtyTrue ); mSwrap = swrap; mTwrap = twrap; mRwrap = rwrap; }

      /// Convenience method to set all wrap parameters at once to the same value.
    void setWrap ( Wrap wrap )
      { setDirty ( DirtyTrue ); mSwrap = mTwrap = mRwrap = wrap; }

      /// This is a hint on how this texture is intended to be used.  Loaders might do the right
      /// thing in filling these in, and an application generally needs to set up the corresponding
      /// programs and uniforms to process these things correctly.
      /// @note All semantics other than diffuse will currently be disabled by default.
      /// @note In the future, loader properties will allow these to be turned on by default.
    void setSemantic ( Semantic semantic = Diffuse ) { mSemantic = semantic; }
    Semantic getSemantic () const { return mSemantic; }

		void setPriority ( float priorityIn = 0.5f ) { setDirty ( DirtyTrue ); mPriority = priorityIn; }
		float getPriority () const { return mPriority; }

// 		void setResize ( Resize resizeIn = ResizeNone ) { mResize = resizeIn; }
// 		Resize getResize () const { return mResize; }
    
 
  protected:
    
  private:
		typedef pni::pstd::autoRef< img::base > ImgRef;
    typedef std::vector< ImgRef > ImgVec;

    ImgVec mImgs;
  
		float mPriority;
 		float mAnisotropy;
		Target mTarget;
		MinFilter mMinFilter;
		MagFilter mMagFilter;
		Wrap mSwrap;
		Wrap mTwrap;
    Wrap mRwrap;
    Semantic mSemantic;
		mutable Dirty mDirty;
// 		Resize mResize;
    

  // interface from state
  public:
    virtual void accept ( stateDd* pDd ) const { pDd->dispatch ( this ); }
    virtual state* dup () const { return new texture ( *this ); }
    
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenetexture_h


