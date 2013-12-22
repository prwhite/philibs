/////////////////////////////////////////////////////////////////////
//
//    class: vbo
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenevbo_h
#define scenescenevbo_h

/////////////////////////////////////////////////////////////////////

#include "scenegeom.h"
#include "sceneattr.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class vbo :
    public attr
{
    public:
        vbo();
        virtual ~vbo();
//         vbo(vbo const& rhs);
//         vbo& operator=(vbo const& rhs);
//         bool operator==(vbo const& rhs) const;
        
        
      void bind ( geom* const pGeom );
      
      void config ( geom* const pGeom );
      
    protected:
      unsigned int mAttrId;
      unsigned int mIndId;
      unsigned int mArrObjId;
      
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

#endif // scenescenevbo_h


