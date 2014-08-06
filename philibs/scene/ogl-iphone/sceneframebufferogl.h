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

    virtual void captureDefaultFb ();
  
  protected:
    
  private:
    

    // interface from scene::framebuffer
  public:

    virtual void bind () override;

    virtual void bind ( 
        framebuffer::TextureImageId colorDest,
        framebuffer::TextureImageId depthDest,
        framebuffer::TextureImageId stencilDest ) override;
  
    virtual void finish ();
  
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneframebufferogl_h


