/////////////////////////////////////////////////////////////////////
//
//    file: scenefbo.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenefbo.h"
#include "scenetexobj.h"

#include <functional>

#include "sceneogl.h"
#include <OpenGLES/ES2/glext.h>

/** Resources:

Overviews/specs:
https://developer.apple.com/library/IOs/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/WorkingwithEAGLContexts/WorkingwithEAGLContexts.html#//apple_ref/doc/uid/TP40008793-CH103-SW1
http://http.download.nvidia.com/developer/presentations/2005/GDC/OpenGL_Day/OpenGL_FrameBuffer_Object.pdf
http://processors.wiki.ti.com/index.php/Render_to_Texture_with_OpenGL_ES
http://luugiathuy.com/2011/09/create-frame-buffer-object-opengl/

Message boards:
http://www.opengl.org/discussion_boards/showthread.php/175868-Render-to-texture
http://mickyd.wordpress.com/2012/05/20/creating-render-to-texture-secondary-framebuffer-objects-on-ios-using-opengl-es-2/
http://blog.shayanjaved.com/2011/05/13/android-opengl-es-2-0-render-to-texture/

*/

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

fbo::fbo()
{
  
}

fbo::~fbo()
{
  clear();
}

//fbo::fbo(fbo const& rhs)
//{
//    
//}
//
//fbo& fbo::operator=(fbo const& rhs)
//{
//    
//    return *this;
//}
//
//bool fbo::operator==(fbo const& rhs) const
//{
//    
//    return false;
//}

void fbo::init ()
{
    // Can't glGenFramebuffers, etc., here because we don't know if we
    // need one yet (e.g., we are wrapping the OS provided framebuffer).
}

void fbo::clear ()
{
  glDeleteFramebuffers(1, &mFramebufferId);
  glDeleteRenderbuffers(1, &mRenderbufferColorId);
  glDeleteRenderbuffers(1, &mRenderbufferDepthId);
  glDeleteRenderbuffers(1, &mRenderbufferStencilId);
  
  mFramebufferId = 0;
  mRenderbufferColorId = 0;
  mRenderbufferDepthId = 0;
  mRenderbufferStencilId = 0;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

fbo* fbo::getOrCreate ( framebuffer const* pFb )
{
    // get or create fbo object for this framebuffer
  if ( fbo* pObj = static_cast< fbo* > ( pFb->getTravData ( Draw ) ) )
  {
    pObj->config ( pFb );
    return pObj;
  }
  else
  {
    pObj = new fbo;
    pObj->config ( pFb );
    const_cast< framebuffer* > ( pFb )->setTravData ( Draw, pObj );
    return pObj;
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#pragma mark - Anon namespace for helper gl mapping functions

namespace {

GLenum colorAttachmentToGlInternalFormat ( framebuffer::ColorAttachment val )
{
  switch ( val )
  {
    case framebuffer::ColorAttachmentNone:          return GL_RGBA4;  // Bad... but this will also get caught elsewhere
    case framebuffer::ColorAttachmentRGBA4:         return GL_RGBA4;
    case framebuffer::ColorAttachmentRGB5_A1:       return GL_RGB5_A1;
    case framebuffer::ColorAttachmentRGB565:        return GL_RGB565;
    case framebuffer::ColorAttachmentRGB8:          return GL_RGB8_OES;
    case framebuffer::ColorAttachmentRGBA8:         return GL_RGBA8_OES;
    case framebuffer::ColorAttachmentBGRA8:         return GL_BGRA8_EXT;
  }
}

GLenum depthAttachmentToGlInternalFormat ( framebuffer::DepthAttachment val )
{
  switch ( val )
  {
    case framebuffer::DepthAttachmentNone:          return GL_DEPTH_COMPONENT16;  // Bad... but this will also get caught elsewhere
    case framebuffer::DepthAttachment16:            return GL_DEPTH_COMPONENT16;
    case framebuffer::DepthAttachment24:            return GL_DEPTH_COMPONENT24_OES;
    case framebuffer::DepthAttachment24Stencil8:    return GL_DEPTH24_STENCIL8_OES;
    case framebuffer::DepthAttachment32:            return GL_DEPTH_COMPONENT32_OES;
  }
}

GLenum stencilAttachmentToGlInternalFormat ( framebuffer::StencilAttachment val )
{
  switch ( val )
  {
    case framebuffer::StencilAttachmentNone:        return GL_STENCIL_INDEX8;  // Bad... but this will also get caught
//    case framebuffer::StencilAttachmentI1:          return GL_STENCIL_INDEX1;
//    case framebuffer::StencilAttachmentI4:          return GL_STENCIL_INDEX4;
    case framebuffer::StencilAttachmentI8:          return GL_STENCIL_INDEX8;
  }
}

  // TODO: Duped from scenetexobj.cpp... should be in common, shared GL code.
inline
int imageIdToGlTargetParam ( texture::Target target, texture::ImageId id )
{
  if ( target == texture::Tex2DTarget )
  {
    return GL_TEXTURE_2D;
  }
  else  // Until we have another target type, the else is for cube maps.
  {
    switch ( id )
    {
//      default:
      case texture::CubePosXImg: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
      case texture::CubeNegXImg: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
      case texture::CubePosYImg: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
      case texture::CubeNegYImg: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
      case texture::CubePosZImg: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
      case texture::CubeNegZImg: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
      case texture::NoImage: return GL_FALSE; // HACK: Totally bogus, but should never get hit.
    }
  }
}


} // end namespace anonymous

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#pragma mark - Methods that modify gl state

bool fbo::verify ( framebuffer const* pFb )
{
  bind ( pFb );

    // check the frame buffer
  GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if ( status != GL_FRAMEBUFFER_COMPLETE)
  {
    PNIDBGSTR("Frame buffer not complete with status: " << status);
    return false;
  }
  else
    return true;
}

  // The conditional logic about what buffers exist and need to be operated
  // on is a recurring theme... so it's boiled out here, and all that is needed
  // is to set up the 8 funcs that would be invoked for a given operation by
  // using the configFuncs arg.  This is a bit over-the-top C++11 stuff...
  // but it seems to be a concise way to do this with minimal code duplication.
void fbo::dispatchFuncs(framebuffer::spec const& spec, configFuncs const& funcs, bool noisy)
{
  for ( size_t num = 0; num < framebuffer::NumColorAttachments; ++num )
  {
    if ( ( spec.mColorType[ num ] == framebuffer::Texture ) )
      funcs.mColorTexture(num);
    else if ( spec.mColorType[ num ] == framebuffer::Renderbuffer )
      funcs.mColorRenderbuffer(num);
    else if ( noisy )
      PNIDBGSTR("no color attachment type set");
  }
  
    // Handle combined depth/stencil specially... not sure if this will
    // fly with texture, but it will with renderbuffer.
  if ( spec.mDepthAttachment == framebuffer::DepthAttachment24Stencil8 )
  {
    if ( spec.mDepthType == framebuffer::Texture )
      funcs.mDepth248Texture();
    else if ( spec.mDepthType == framebuffer::Renderbuffer )
      funcs.mDepth248Renderbuffer();
    else if ( noisy )
      PNIDBGSTR("no depth attachment type set");
  }
  else
  {
    if ( spec.mDepthType == framebuffer::Texture )
      funcs.mDepthTexture();
    else if ( spec.mDepthType == framebuffer::Renderbuffer )
      funcs.mDepthRenderbuffer();
    else if ( noisy )
      PNIDBGSTR("no depth attachment type set");

    if ( spec.mStencilType == framebuffer::Texture )
      funcs.mStencilTexture();
    else if ( spec.mStencilType == framebuffer::Renderbuffer )
      funcs.mStencilRenderbuffer();
    else if ( noisy )
      PNIDBGSTR("no stencil attachment type set");
  }
}

void fbo::bind ( framebuffer const* pFb )
{
  glBindFramebuffer ( GL_FRAMEBUFFER, mFramebufferId );
}

void fbo::captureDefaultFb ()
{
  GLint defaultFBO;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
  
  mFramebufferId = defaultFBO;
}


void fbo::bind ( framebuffer const* pFb,
    framebuffer::TextureImageId colorDest,
    framebuffer::TextureImageId depthDest ,
    framebuffer::TextureImageId stencilDest )
{
  glPushGroupMarkerEXT(0, "fbo::bind (public)");

CheckGLError
  bind ( pFb );
CheckGLError
  framebuffer::spec const& spec = pFb->getSpec();

    // For color attachment texture... from
    // https://developer.apple.com/library/IOs/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/WorkingwithEAGLContexts/WorkingwithEAGLContexts.html#//apple_ref/doc/uid/TP40008793-CH103-SW1
  
    // It goes like this ex... but glTexImage2D will be called from the texobj::getOrCreate call.
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);


  dispatchFuncs(spec, configFuncs {
    [pFb, colorDest] ( size_t num )
      {
        if ( texture* pTex = pFb->getColorTextureTarget(num) )
        {
          texObj* pObj = texObj::getOrCreate(pTex);
          pObj->bind(pTex); // just to make sure... generally done by getOrCreate
          GLenum textarget = imageIdToGlTargetParam(pTex->getTarget(), colorDest);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textarget, pObj->getId(), 0);
          pTex->setDirty(texture::DirtyMipMaps); // So it will regen mipmaps after rendering
        }
      },
    [] ( size_t num ) {},
    [pFb, depthDest] ()
      {
        if ( texture* pTex = pFb->getDepthTextureTarget() )
        {
          texObj* pObj = texObj::getOrCreate(pTex);
          pObj->bind(pTex); // just to make sure... generally done by getOrCreate
          GLenum textarget = imageIdToGlTargetParam(pTex->getTarget(), depthDest);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_OES, textarget, pObj->getId(), 0);
          pTex->setDirty(texture::DirtyMipMaps); // So it will regen mipmaps after rendering
        }
      },
    [] () {},
    [pFb, depthDest] ()
      {
        if ( texture* pTex = pFb->getDepthTextureTarget() )
        {
          texObj* pObj = texObj::getOrCreate(pTex);
          pObj->bind(pTex); // just to make sure... generally done by getOrCreate
          GLenum textarget = imageIdToGlTargetParam(pTex->getTarget(), depthDest);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textarget, pObj->getId(), 0);
          pTex->setDirty(texture::DirtyMipMaps); // So it will regen mipmaps after rendering
        }
      },
    [] () {},
    [pFb, stencilDest] ()
      {
        if ( texture* pTex = pFb->getStencilTextureTarget() )
        {
          texObj* pObj = texObj::getOrCreate(pTex);
          pObj->bind(pTex); // just to make sure... generally done by getOrCreate
          GLenum textarget = imageIdToGlTargetParam(pTex->getTarget(), stencilDest);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, textarget, pObj->getId(), 0);
          pTex->setDirty(texture::DirtyMipMaps); // So it will regen mipmaps after rendering
        }
      },
    [] () {}
  } );
  
CheckGLError

    // gles doesn't have glDrawBuffer... it imiplicitly always binds to the
    // current color attachment... front or back.
#ifdef GL_VERSION_1_0
  glDrawBuffer ( spec.mDestinationBuffer == framebuffer::Front ? GL_FRONT : GL_BACK );
#endif // GL_VERSION_1_0

  glPopGroupMarkerEXT();

CheckGLError
}

void fbo::finish ( framebuffer const* pFb )
{
  glPushGroupMarkerEXT(0, "fbo::discard start");

  framebuffer::spec const& spec = pFb->getSpec();

  dispatchFuncs(spec, configFuncs {
    [pFb] ( size_t num ) {
      if ( texture* pTex = pFb->getColorTextureTarget(num) )
        texObj::getOrCreate(pTex);  // Force mipmap generation now
    },
    [pFb] ( size_t num ) {
        GLenum which = GL_COLOR_ATTACHMENT0;
        glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, &which); },
    [pFb] () {
      if ( texture* pTex = pFb->getDepthTextureTarget() )
        texObj::getOrCreate(pTex);  // Force mipmap generation now
    },
    [pFb] () {
        GLenum which = GL_DEPTH_ATTACHMENT;
        glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, &which ); },
    [pFb] () {
      if ( texture* pTex = pFb->getDepthTextureTarget() )
        texObj::getOrCreate(pTex);  // Force mipmap generation now
    },
    [pFb] () {
        GLenum which = GL_DEPTH_ATTACHMENT;
        glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, &which); },
    [pFb] () {
      if ( texture* pTex = pFb->getStencilTextureTarget() )
        texObj::getOrCreate(pTex);  // Force mipmap generation now
    },
    [pFb] () {
        GLenum which = GL_STENCIL_ATTACHMENT;
        glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, &which); }
  } );
  
  glPopGroupMarkerEXT();
}

void fbo::initBuffers ( framebuffer::spec const& spec )
{
    // We don't create a framebuffer in init like other gl object classes...
    // so we do it deferred here.  If it's not 0, then we've already created it.
    // We never get here for Type == Default because that is handled upstream.

  if ( mFramebufferId != 0 )
    return;   // Early return!!! Precondition
  
  glGenFramebuffers(1, &mFramebufferId);
  
  dispatchFuncs(spec, configFuncs {
    [] ( size_t num ) {},
    [&] ( size_t num ) { glGenRenderbuffers(1, &mRenderbufferColorId); },
    [] () {},
    [&] () { glGenRenderbuffers(1, &mRenderbufferDepthId); },
    [] () {},
    [&] () { glGenRenderbuffers(1, &mRenderbufferDepthId); },
    [] () {},
    [&] () { glGenRenderbuffers(1, &mRenderbufferStencilId); }
  } );
CheckGLError
}

void fbo::configBuffers(framebuffer const* pFb, framebuffer::spec const& spec )
{
  dispatchFuncs(spec, configFuncs {
    [] ( size_t num ) { },
    [&] ( size_t num )  // Color renderbuffer
      {
        glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferColorId);
        glRenderbufferStorage(GL_RENDERBUFFER, colorAttachmentToGlInternalFormat(spec.mColorAttachment[ num ]), spec.mWidth, spec.mHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRenderbufferColorId);
      },
    [] () {},
    [&] ()  // Combined Depth24Stencil8 renderbuffer
      {
        glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferDepthId);
        glRenderbufferStorage(GL_RENDERBUFFER, depthAttachmentToGlInternalFormat(spec.mDepthAttachment), spec.mWidth, spec.mHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_OES, GL_RENDERBUFFER, mRenderbufferDepthId);
      },
    [] () {},
    [&] ()  // Depth renderbuffer
      {
        glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferDepthId);
        glRenderbufferStorage(GL_RENDERBUFFER, depthAttachmentToGlInternalFormat(spec.mDepthAttachment), spec.mWidth, spec.mHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderbufferDepthId);
      },
    [] () {},
    [&] ()  // Stencil renderbuffer
      {
        glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferStencilId);
        glRenderbufferStorage(GL_RENDERBUFFER, stencilAttachmentToGlInternalFormat(spec.mStencilAttachment), spec.mWidth, spec.mHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderbufferStencilId);
      }
  } );
CheckGLError
}

#pragma mark - config method... main entry point from client classes

void fbo::config ( framebuffer const* pFb )
{
  framebuffer::spec const& spec = pFb->getSpec ();
  
    // We don't do any configs on the default framebuffer... but
    // we do want to make sure it's bound.
    // HACK: This is a special short-circuit right now to use the default
    // framebuffer if the first color attachment type is Default.
  if ( spec.mColorType[ 0 ] == framebuffer::Default )
  {
      // Don't bind to zero... that might not be what we want
      // in the case of the OS-provided fb.  See captureDefaultFb.
//    bind ( pFb );
    return;   // Early return!!!
  }

  if ( ! pFb->getDirty() && ! pFb->getTexturesDirty() )
    return;   // Early return!!!

    // We do latent init of GL objects in this class...
  clear();            // So we can be re-init'd.
  initBuffers(spec);
  
  glLabelObjectEXT(GL_FRAMEBUFFER, mFramebufferId, 0, pFb->getName().c_str());
  
    // ... so we can't bind until that's done
  bind ( pFb );
  
  configBuffers(pFb, spec);
  
  verify(pFb);
  
  pFb->setDirty(false);
}


/////////////////////////////////////////////////////////////////////
// overrides from scene::attr



/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


