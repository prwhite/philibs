// ///////////////////////////////////////////////////////////////////
//
//  class: uniobj
//
// ///////////////////////////////////////////////////////////////////

#ifndef sceneuniobj_h
#define sceneuniobj_h

// ///////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "sceneattr.h"
#include "sceneogl.h"
#include "sceneprogobj.h"
#include "sceneuniform.h"
#include <vector>

// ///////////////////////////////////////////////////////////////////

namespace scene {
  
    class uniform;
  
// ///////////////////////////////////////////////////////////////////

class uniobj :
  public attr
{
  public:

    static uniobj* getOrCreate ( uniform const* pUni, progObj* pProgObj );
  
    struct binding
    {
      GLint mLoc;
      GLuint mProgId;
      uniform::binding const* mBinding;
    };
  
    void bind ();

  protected:
  
    void config ( uniform const* pUni, progObj* pProgObj );
    void apply ();

    using Bindings = std::vector < binding >;
    Bindings mBindings;
    progObj* mLastProg = nullptr;  // weak reference... we don't evaluate it
  
  private:
  
    // Inherited
  public:
      // TRICKY HACK: What happens if we do this?
    virtual attr* dup () const { return 0; }
  

};

// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // sceneuniobj_h
