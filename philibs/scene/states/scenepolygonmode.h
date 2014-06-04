/////////////////////////////////////////////////////////////////////
//
//    class: polygonMode
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenepolygonmode_h
#define scenescenepolygonmode_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class polygonMode :
  public state
{
  public:
    polygonMode();
    virtual ~polygonMode();
//     polygonMode(polygonMode const& rhs);
//     polygonMode& operator=(polygonMode const& rhs);
//     bool operator==(polygonMode const& rhs) const;
    

		enum Mode { Point, Line, Fill };
		void setMode ( Mode modeIn = Fill ) { mMode = modeIn; }
		Mode getMode () const { return mMode; }


  protected:
    
  private:
    Mode mMode;

  // interface from state
  public:
    virtual polygonMode* dup () const { return new polygonMode ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenepolygonmode_h


