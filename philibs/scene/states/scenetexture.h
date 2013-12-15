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


		// NOTE: cube targets start at zero for a reason (see cubeMap)
// 		enum Target { CubePosX, CubeNegX, CubePosY, CubeNegY, CubePosZ, CubeNegZ, Tex1D, Tex2D, Tex3D };
		enum Target { Tex2D };
		enum Dirty { DirtyFalse, DirtyTrue };
		enum MinFilter { 
        MinNearest, 
        MinLinear, 
        MinNearestMipNearest, 
        MinLinearMipNearest, 
        MinNearestMipLinear, 
        MinLinearMipLinear 
    };
		enum MagFilter { MagNearest, MagLinear };
// 		enum Wrap { Clamp, Repeat, ClampToEdge };
		enum Wrap { Repeat };
// 		enum Resize { ResizeNone, Fill, Smaller, Bigger };

		void setTarget ( Target targetIn = Tex2D ) { mTarget = targetIn; }
		Target getTarget () const { return mTarget; }

		// texture sources
		void setImage ( img::base* pImg ) { setDirty ( DirtyTrue ); mImg = pImg; }
		img::base* getImage () const { return mImg.get (); }

		// use this when texture parameters/data changes
    // TRICKY: setDirty is const... clearing dirty occurs for const
    // objects... blah blah blah.
		void setDirty ( Dirty dirtyIn = DirtyTrue )const { mDirty = dirtyIn; }
		Dirty getDirty () const { return mDirty; }

		void setMinFilter ( MinFilter minFilterIn = MinLinear ) { setDirty ( DirtyTrue ); mMinFilter = minFilterIn; }
		MinFilter getMinFilter () const { return mMinFilter; }

		void setMagFilter ( MagFilter magFilterIn = MagLinear ) { setDirty ( DirtyTrue ); mMagFilter = magFilterIn; }
		MagFilter getMagFilter () const { return mMagFilter; }

// 		void setMaxAnisotropy ( float val = 0.0f ) { mAnisotropy = val; } // [0,1] = [Default,HWMax]
// 		float getMaxAnisotropy () const { return mAnisotropy; }

		void setSWrap ( Wrap swrapIn = Repeat ) { setDirty ( DirtyTrue ); mSwrap = swrapIn; }
		Wrap getSWrap () const { return mSwrap; }

		void setTWrap ( Wrap twrapIn = Repeat ) { setDirty ( DirtyTrue ); mTwrap = twrapIn; }
		Wrap getTWrap () const { return mTwrap; }

		void setPriority ( float priorityIn = 0.5f ) { setDirty ( DirtyTrue ); mPriority = priorityIn; }
		float getPriority () const { return mPriority; }

// 		void setResize ( Resize resizeIn = ResizeNone ) { mResize = resizeIn; }
// 		Resize getResize () const { return mResize; }
    
 
  protected:
    
  private:
		pni::pstd::autoRef< img::base > mImg;

		float mPriority;
// 		float mAnisotropy;
		Target mTarget;
		MinFilter mMinFilter;
		MagFilter mMagFilter;
		Wrap mSwrap;
		Wrap mTwrap;
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


