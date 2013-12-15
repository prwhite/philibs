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
        
        
      void bind ( texture const* pTex );
      
      void config ( texture const* pTex );
      
    protected:
      unsigned int mId;
      
      void init ();
      void clear ();
        
    private:
        

	// interface from node::travData
	public:
  
    // TRICKY HACK: What happens if we do this?
    virtual attr* dup () const { return 0; }
    
	protected:

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenetexobj_h


