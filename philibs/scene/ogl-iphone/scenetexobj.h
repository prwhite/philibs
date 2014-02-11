/////////////////////////////////////////////////////////////////////
//
//    class: texObj
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenetexobj_h
#define scenescenetexobj_h

/////////////////////////////////////////////////////////////////////

#include "scenetexture.h"
#include "sceneattr.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class texObj :
    public attr
{
    public:
        texObj();
        virtual ~texObj();
//         texObj(texObj const& rhs);
//         texObj& operator=(texObj const& rhs);
//         bool operator==(texObj const& rhs) const;
  
      static texObj* getOrCreate ( texture const* textureIn );
        
      void bind ( texture const* pTex );
      
      void config ( texture const* pTex );
  
      typedef uint32_t Handle;
  
      Handle getId () const { return mId; }
      
    protected:
      unsigned int mGlTextureTarget = 0;
      Handle mId = 0;
      
      void init ();
      void clear ();
      void configOneTextureImg ( texture const* pTex, texture::ImageId imgId, img::base const* pImg );
  
    private:
        

    // interface from node::attr
	public:
  
      // TRICKY HACK: What happens if we do this?
    virtual attr* dup () const { return 0; }
    
	protected:

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenetexobj_h


