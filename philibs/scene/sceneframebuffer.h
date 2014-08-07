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

    typedef int32_t Dim;

    typedef texture::Target TextureTarget;    /// e.g., NoTarget, Tex2DTarget, CubeMapTarget
    typedef texture::ImageId TextureImageId;  /// e.g., NoImage, Tex2DImage, CubePosXImg...

    enum Type {
      AttachNone,           /// No attachment
      AttachDefault,        /// System provided default, already config'd
      AttachRenderbuffer,   /// Render buffer attachment
      AttachTexture         /// Texture (2D or cube map) attachment
    };

    enum ColorAttachment {
      ColorAttachmentNone,
      ColorAttachmentRGBA4,
      ColorAttachmentRGB5_A1,
      ColorAttachmentRGB565,
      ColorAttachmentRGB8,
      ColorAttachmentRGBA8,
      ColorAttachmentBGRA8,
      ColorAttachmentR8,
      ColorAttachmentRG88
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

    using TexRef = pni::pstd::autoRef< texture >;
#ifdef PHIUSEGLES30
    static uint32_t const NumColorAttachments = 4;  // Is this reasonable from the
                                                    // MRT extension spec?  Seems so in
                                                    // GLES 3.0, but there are a lot more
                                                    // in GL proper.
#else // PHIUSEGLES30
    static uint32_t const NumColorAttachments = 1;
#endif // PHIUSEGLES30

    static_assert(NumColorAttachments==1,"not tested with NumColorAttachments > 1");
  
    struct spec
    {
      TextureTarget mTextureTarget            = texture::NoTarget;
      Type mColorType[ NumColorAttachments ]  = { AttachDefault };
      ColorAttachment mColorAttachment[ NumColorAttachments ] = { ColorAttachmentNone };
      Type mDepthType                         = AttachDefault;
      DepthAttachment mDepthAttachment        = DepthAttachmentNone;
      Type mStencilType                       = AttachNone;
      StencilAttachment mStencilAttachment    = StencilAttachmentNone;
      DestinationBuffer mDestinationBuffer    = Back;

        // Not texture size... but framebuffer size... might need to be
        // multiple of OS framebuffer size.
      Dim mWidth                         = 256;
      Dim mHeight                        = 256;
    };
  
    struct textureTargets
    {
      friend class framebuffer;
      
      public:
        void setColorTextureTarget ( size_t which, texture* pTex )
          { mColorTex[ which ] = pTex; }
        texture* getColorTextureTarget ( size_t which ) const
          { return mColorTex[ which ].get (); }

        void setDepthTextureTarget ( texture* pTex )
          { mDepthTex = pTex; }
        texture* getDepthTextureTarget () const
          { return mDepthTex.get (); }
      
        void setStencilTextureTarget ( texture* pTex )
          { mStencilTex = pTex; }
        texture* getStencilTextureTarget () const
          { return mStencilTex.get (); }

        void collectRefs ( pni::pstd::refCount::Refs& refs ) const
        {
          for(auto& iter : mColorTex)
            refs.push_back(iter.get());
          refs.push_back(mDepthTex.get());
          refs.push_back(mStencilTex.get());
        }

      private:
          TexRef mColorTex[ NumColorAttachments ];
          TexRef mDepthTex;
          TexRef mStencilTex;
    };

    void setSpec ( spec const& fbspec )
      { setDirty (); mSpec = fbspec; }
    spec const& getSpec () const { return mSpec; }
    spec& specOp () { setDirty (); return mSpec; }
  
      /// For bulk setting of texture targets.  Generally used by renderSinks
      /// so that framebuffers can be re-used with different texture targets.
    void setTextureTargets ( textureTargets const& vals ) { /* setDirty (); */ mTextureTargets = vals; }
    textureTargets& textureTargetOp () { setDirty(); return mTextureTargets; }
    textureTargets const& getTextureTargets () const { return mTextureTargets; }

      // TODO: Retrieve color/depth/stencil render buffer contents
      // TODO: Query available frame buffer formats
      // TODO: Signal to implicitly create/share GL contexts

    void setDirty ( bool val = true ) const { mDirty = val; }
    bool getDirty () const { return mDirty; }
    void clearDirty () { /* clean up state here */; mDirty = false; }
  
    bool getTexturesDirty ( texture::Dirty dirty = texture::DirtyTrue ) const
      {
        return ( mTextureTargets.mColorTex[ 0 ] && ( mTextureTargets.mColorTex[ 0 ]->getDirty () & dirty ) )
            || ( mTextureTargets.mDepthTex && ( mTextureTargets.mDepthTex->getDirty () & dirty ) )
            || ( mTextureTargets.mStencilTex && ( mTextureTargets.mStencilTex->getDirty () & dirty ) );
      }

      /// Simple rebind of framebuffer.
    virtual void bind () = 0;

      /// Bind this framebuffer, with all of its associated texture or
      /// renderbuffers, according to the #TextureImageId passed in for each
      /// attachement.
      /// This does a more significant rebind of attached textures than the other
      /// bind.
      /// @note: TODO: This doesn't do a good job of representing n color attachments.
      /// @note: This is generally used only for RT cube maps as they need re-
      /// attachment of textures on a re-used framebuffer.
    virtual void bind (
        framebuffer::TextureImageId colorDest,
        framebuffer::TextureImageId depthDest,
        framebuffer::TextureImageId stencilDest ) = 0;

      /// Purge resources associated with the framebuffer when a rendering
      /// pass is done.  This provides the h/w with an opportunity to _not_
      /// backup a vram buffer to main memory.
      /// @note Also, this is where fbo textures will be finalized (and dynamic
      /// mip maps generated).  Allowing textures to do the usual lazy eval
      /// was causing big performance hit because mipmap generation causes
      /// save/restore of framebuffer contents.
    virtual void finish () = 0;

      /// Call this to capture the OS-specific framebuffer ID into the
      /// framebuffer's underlying implementation.  Make sure when this
      /// is called that the system-provided default fb is bound... i.e.,
      /// call this before setting up any other fbs.
    virtual void captureDefaultFb () = 0;

    void setName ( std::string const& name ) { mName = name; }
    std::string const& getName () const { return mName; }

  protected:
      
  private:
      spec mSpec;
      textureTargets mTextureTargets;
  
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


