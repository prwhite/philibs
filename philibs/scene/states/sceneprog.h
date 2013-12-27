/////////////////////////////////////////////////////////////////////
//
//    class: prog
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneprog_h
#define scenesceneprog_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class prog :
  public state
{
  public:
    prog();
    virtual ~prog();
//     prog(prog const& rhs);
//     prog& operator=(prog const& rhs);
//     bool operator==(prog const& rhs) const;
    
  protected:
    
  private:
  

  // interface from state
  public:
    virtual state* dup () const { return new prog ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneprog_h


