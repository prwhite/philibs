/////////////////////////////////////////////////////////////////////
//
//    class: framebuffer
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneframebuffer_h
#define scenesceneframebuffer_h

/////////////////////////////////////////////////////////////////////

#include "scenetexture.h"
#include "scenetypes.h"

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

  /**
    This class represents a GL framebuffer object.  This allows the
    app to set up render to texture for things like full framebuffer
    post-processing, real-time reflections, shadow buffers, etc.
    This class is best used in concert with the renderSink class, which
    manages the render order and settings for all passes for a whole 
    scene.
    Here are the steps to execute if you want to do simple render-to-
    texture:
    1. Create a dummy image of the appropriate format and size.
    2. Create a texture and associate the image with it.
    3. Create a framebuffer::spec and set the members to include
       attaching the color output to a texture, and depth to a renderbuffer.
    4. Create a framebuffer instance, and associate the spec with the
       framebuffer via the specOp method.
    5. Associate the previously created texture with the color texture target
       on the framebuffer.  For fun, you can verify the framebuffer is complete
       at this point (with the verify method).
    6. In your frame loop, make sure to call framebuffer::bind to have the 
       framebuffer instance become the rendering destination.
    This example leaves out a lot of details, like re-binding the default 
    framebuffer and using the texture that we rendered into in another 
    rendering pass.
    @note You will generally use a derived framebuffer class, like 
    framebufferOgl, which actually implements the interaction with GLES.
  */
class framebuffer :
    public pni::pstd::refCount,
    public travDataContainer
{
  public:
    framebuffer() = default;
    virtual ~framebuffer() = default;
    framebuffer(framebuffer const& rhs) = delete;
    framebuffer& operator=(framebuffer const& rhs) = delete;
//    bool operator==(framebuffer const& rhs) const = default;

    typedef int32_t SizeType;

    typedef texture::Target TextureTarget;    /// e.g., NoTarget, Tex2DTarget, CubeMapTarget
    typedef texture::ImageId TextureImageId;  /// e.g., NoImage, Tex2DImage, CubePosXImg...

    enum Type {
      Default,
      Renderbuffer,
      Texture
    };

    enum ColorAttachment {
      ColorAttachmentNone,
      ColorAttachmentRGBA4,
      ColorAttachmentRGB5_A1,
      ColorAttachmentRGB565,
      ColorAttachmentRGB8,
      ColorAttachmentRGBA8,
      ColorAttachmentBGRA8,
        // More from GL_EXT_texture_storage(?)
    };
  
    enum DepthAttachment {
      DepthAttachmentNone,
      DepthAttachment16,
      DepthAttachment24,    // from GL_DEPTH_COMPONENT24_OES... must be used with
                            // StencilAttachmentI8.
      DepthAttachment24Stencil8, // Leave stencil attachment empty when using this.
      DepthAttachment32     // Probably not implemented/tested currently.
    };
  
      /// @note Standalone stencil formats are rumored not to be supported on
      /// iOS 5+.  You must used the combined DepthAttachment24Stencil8 format.
    enum StencilAttachment {
      StencilAttachmentNone,
//      StencilAttachmentI1,  // Not available in GLES2/glext.h???
//      StencilAttachmentI4,  // Not available in GLES2/glext.h???
      StencilAttachmentI8
    };
  
    enum DestinationBuffer {
      Front,
      Back
    };

#ifdef PHIUSEGLES30
    static uint32_t const NumColorAttachments = 4;  // Is this reasonable from the
                                                    // MRT extension spec?  Seems so in
                                                    // GLES 3.0, but there are a lot more
                                                    // in GL proper.
#else // PHIUSEGLES30
    static uint32_t const NumColorAttachments = 1;
#endif // PHIUSEGLES30
  
    struct spec
    {
      TextureTarget mTextureTarget            = texture::NoTarget;
      Type mColorType[ NumColorAttachments ]  = { Default };
      ColorAttachment mColorAttachment[ NumColorAttachments ] = { ColorAttachmentNone };
      Type mDepthType                         = Default;
      DepthAttachment mDepthAttachment        = DepthAttachmentNone;
      Type mStencilType                       = Default;
      StencilAttachment mStencilAttachment    = StencilAttachmentNone;
      DestinationBuffer mDestinationBuffer    = Back;

        // Not texture size... but framebuffer size... might need to be
        // multiple of OS framebuffer size.
      SizeType mWidth                         = 256;
      SizeType mHeight                        = 256;
    };

    void setSpec ( spec const& fbspec )
      { setDirty (); mSpec = fbspec; }
    spec const& getSpec () const { return mSpec; }
    spec& specOp () { setDirty (); return mSpec; }
  
    void setColorTextureTarget ( size_t which, texture* pTex )
      { setDirty (); mColorTex[ which ] = pTex; }
    texture* getColorTextureTarget ( size_t which ) const
      { return mColorTex[ which ].get (); }

    void setDepthTextureTarget ( texture* pTex )
      { setDirty (); mDepthTex = pTex; }
    texture* getDepthTextureTarget () const
      { return mDepthTex.get (); }
  
    void setStencilTextureTarget ( texture* pTex )
      { setDirty (); mStencilTex = pTex; }
    texture* getStencilTextureTarget () const
      { return mStencilTex.get (); }

      // TODO: Retrieve color/depth/stencil render buffer contents
      // TODO: Query available frame buffer formats
      // TODO: Signal to implicitly create/share GL contexts

    void setDirty ( bool val = true ) const { mDirty = val; }
    bool getDirty () const { return mDirty; }
    void clearDirty () { /* clean up state here */; setDirty ( false ); }
  
    bool getTexturesDirty () const
      {
        return ( mColorTex[ 0 ] && mColorTex[ 0 ]->getDirty () )
            || ( mDepthTex && mDepthTex->getDirty () )
            || ( mStencilTex && mStencilTex->getDirty () );
      }


      /// Bind this framebuffer, with all of its associated texture or
      /// renderbuffers, according to the #TextureImageId passed in for each
      /// attachement.
      /// @node: TODO: This doesn't do a good job of representing n color attachments.
    virtual void bind (
        framebuffer::TextureImageId colorDest,
        framebuffer::TextureImageId depthDest,
        framebuffer::TextureImageId stencilDest ) = 0;

      /// Purge resources associated with the framebuffer when a rendering
      /// pass is done.  This provides the h/w with an opportunity to _not_
      /// backup a vram buffer to main memory.
    virtual void discard () = 0;

      /// Call this to capture the OS-specific framebuffer ID into the
      /// framebuffer's underlying implementation.  Make sure when this
      /// is called that the system-provided default fb is bound... i.e.,
      /// call this before setting up any other fbs.
    virtual void captureDefaultFb () = 0;

    void setName ( std::string const& name ) { mName = name; }
    std::string const& getName () const { return mName; }

  protected:
      
  private:
      typedef pni::pstd::autoRef< texture > TexRef;
  
      spec mSpec;
      TexRef mColorTex[ NumColorAttachments ] = { nullptr };
      TexRef mDepthTex                        = nullptr;
      TexRef mStencilTex                      = nullptr;
  
      std::string mName;
  
      mutable bool mDirty                     = true;

  // interface from pni::pstd::refCount
  public:

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
  
  protected:
		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneframebuffer_h


