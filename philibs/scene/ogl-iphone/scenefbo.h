/////////////////////////////////////////////////////////////////////
//
//    class: fbo
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenefbo_h
#define scenescenefbo_h

/////////////////////////////////////////////////////////////////////

#include "sceneattr.h"
#include "sceneframebuffer.h"
#include <functional>

/////////////////////////////////////////////////////////////////////

namespace scene {

class texture;

/////////////////////////////////////////////////////////////////////

class fbo :
    public attr
{
  public:
    fbo();
    virtual ~fbo();
    fbo(fbo const& rhs) = delete;
    fbo& operator=(fbo const& rhs) = delete;
//    bool operator==(fbo const& rhs) const;

    static fbo* getOrCreate ( framebuffer const* pFb );

    bool verify ( framebuffer const* pFb );
  
    void captureDefaultFb ();
  
      /// Simply bind the framebuffer to gl
    void bind ( framebuffer const* pFb );

      /// Bind the framebuffer to gl, along with associating the correct
      /// target drawable (e.g., default fb, texture, or texture cubemap side).
    void bind ( framebuffer const* pFb,
            framebuffer::TextureImageId colorDest,
            framebuffer::TextureImageId depthDest,
            framebuffer::TextureImageId stencilDest );
  
    void finish ( framebuffer const* pFb );

    typedef uint32_t Handle;

  protected:
    Handle mFramebufferId           = 0;
    Handle mRenderbufferColorId     = 0;    // Needs to be array of framebuffer::NumColorAttachments elements for GLES 3.0
    Handle mRenderbufferDepthId     = 0;
    Handle mRenderbufferStencilId   = 0;

    void init ();
    void clear ();
  
    void initBuffers ( framebuffer::spec const& spec );
    void configBuffers ( framebuffer const* pFb, framebuffer::spec const& spec );

      /// Update fbo parameters... if the source fb or textures are "dirty",
      /// then the necessary state will be rebound.  This is generally called
      /// from getOrCreate.
    void config ( framebuffer const* pFb );

      /// @group The set of functions that will be conditionally invoked
      /// when various fbo parameters are being changed or bound.

      // Read the docs in the cpp file for dispatchSpec to understand this group.
    typedef std::function< void ( size_t num ) > ColorFunc;
    typedef std::function< void () > DepthStencilFunc;
    struct configFuncs {
      ColorFunc             mColorTexture;
      ColorFunc             mColorRenderbuffer;
      DepthStencilFunc      mDepth248Texture;
      DepthStencilFunc      mDepth248Renderbuffer;
      DepthStencilFunc      mDepthTexture;
      DepthStencilFunc      mDepthRenderbuffer;
      DepthStencilFunc      mStencilTexture;
      DepthStencilFunc      mStencilRenderbuffer;
    };

    void dispatchFuncs(framebuffer::spec const& spec, configFuncs const& funcs, bool noisy = false);
  
  private:
        

    // interface from scene::attr
	public:

      // TRICKY HACK: What happens if we do this?
    virtual attr* dup () const { return 0; }

	protected:
		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenefbo_h


