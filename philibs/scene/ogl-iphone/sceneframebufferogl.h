/////////////////////////////////////////////////////////////////////
//
//  class: framebufferOgl
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneframebufferogl_h
#define scenesceneframebufferogl_h

/////////////////////////////////////////////////////////////////////

#include "sceneframebuffer.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class framebufferOgl :
    public scene::framebuffer
{
  public:
//    framebufferOgl();
//    virtual ~framebufferOgl();
//    framebufferOgl(framebufferOgl const& rhs);
//    framebufferOgl& operator=(framebufferOgl const& rhs);
//    bool operator==(framebufferOgl const& rhs) const;


      // Strangely public, but not part of the framebuffer abstraction. Huh.
    bool verify ();
  
  protected:
    
  private:
    

    // interface from scene::framebuffer
  public:

    virtual void bind ( 
        framebuffer::TextureImageId colorDest,
        framebuffer::TextureImageId depthDest = texture::NoImage,
        framebuffer::TextureImageId stencilDest = texture::NoImage );
  
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneframebufferogl_h


