/////////////////////////////////////////////////////////////////////
//
//  file: sceneframebufferogl.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneframebufferogl.h"

#include "scenefbo.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

//framebufferOgl::framebufferOgl()
//{
//  
//}
//
//framebufferOgl::~framebufferOgl()
//{
//  
//}
//
//framebufferOgl::framebufferOgl(framebufferOgl const& rhs)
//{
//  
//}
//
//framebufferOgl& framebufferOgl::operator=(framebufferOgl const& rhs)
//{
//  return *this;
//}
//
//bool framebufferOgl::operator==(framebufferOgl const& rhs) const
//{
//  return false;
//}

/////////////////////////////////////////////////////////////////////
// overrides from scene::framebuffer

void framebufferOgl::captureDefaultFb ()
{
  fbo* pFbo = fbo::getOrCreate(this);

  pFbo->captureDefaultFb();
}

void framebufferOgl::bind ()
{
  fbo* pFbo = fbo::getOrCreate(this);
  
  pFbo->bind ( this );
}
  
void framebufferOgl::bind (
    framebuffer::TextureImageId colorDest,
    framebuffer::TextureImageId depthDest,
    framebuffer::TextureImageId stencilDest )
{
  fbo* pFbo = fbo::getOrCreate(this);
  
  pFbo->bind ( this, colorDest, depthDest, stencilDest );
}

void framebufferOgl::finish ()
{
  fbo* pFbo = fbo::getOrCreate(this);
  
  pFbo->finish(this);
}

bool framebufferOgl::verify ()
{
  fbo* pFbo = fbo::getOrCreate(this);

  return pFbo->verify ( this );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


