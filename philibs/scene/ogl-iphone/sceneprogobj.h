/////////////////////////////////////////////////////////////////////
//
//    class: progObj
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneprogObj_h
#define scenesceneprogObj_h

/////////////////////////////////////////////////////////////////////

#include "sceneprog.h"
#include "sceneattr.h"

#include "sceneogl.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class progObj :
    public attr
{
    public:
        progObj();
        virtual ~progObj();
//         progObj(progObj const& rhs);
//         progObj& operator=(progObj const& rhs);
//         bool operator==(progObj const& rhs) const;
        
        
      void bind ( prog const* pData );
      
      void config ( prog const* pData );
  
      bool validate () const;

      GLuint getPipelineHandle () const { return mPipeline; }
      GLuint getVertexProgHandle () const { return mVertProg; }
      GLuint getFragmentProgHandle () const { return mFragProg; }
      
    protected:
      
      void init ();
      void clear ();
        
    private:
      GLuint mPipeline = 0;
      GLuint mVertProg = 0;
      GLuint mFragProg = 0;


	// interface from node::travData
	public:
  
    // TRICKY HACK: What happens if we do this?
    virtual attr* dup () const { return 0; }
    
	protected:

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneprogObj_h


