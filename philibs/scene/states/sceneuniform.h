/////////////////////////////////////////////////////////////////////
//
//    class: uniform
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneuniform_h
#define scenesceneuniform_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class uniform :
  public state
{
  public:
    uniform();
    virtual ~uniform();
//     uniform(uniform const& rhs);
//     uniform& operator=(uniform const& rhs);
//     bool operator==(uniform const& rhs) const;
  
  protected:
    
  private:
    

  // interface from state
  public:
    virtual state* dup () const { return new uniform ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneuniform_h


