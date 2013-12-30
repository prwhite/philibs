/////////////////////////////////////////////////////////////////////
//
//    class: progobj
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneprogobj_h
#define scenesceneprogobj_h

/////////////////////////////////////////////////////////////////////

#include "sceneprog.h"
#include "sceneattr.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class progobj :
    public attr
{
    public:
        progobj();
        virtual ~progobj();
//         progobj(progobj const& rhs);
//         progobj& operator=(progobj const& rhs);
//         bool operator==(progobj const& rhs) const;
        
        
      void bind ( prog const* pData );
      
      void config ( prog const* pData );
      
    protected:
      
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

#endif // scenesceneprogobj_h


