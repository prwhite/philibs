/////////////////////////////////////////////////////////////////////
//
//    class: sceneNodeRotAxis
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimscenenoderotaxis_h
#define animanimscenenoderotaxis_h

/////////////////////////////////////////////////////////////////////

#include "animbase.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
  
/////////////////////////////////////////////////////////////////////

class sceneNodeRotAxis :
  public base
{
  public:
    sceneNodeRotAxis();
    virtual ~sceneNodeRotAxis();
    sceneNodeRotAxis(sceneNodeRotAxis const& rhs);
    sceneNodeRotAxis& operator=(sceneNodeRotAxis const& rhs);
    bool operator==(sceneNodeRotAxis const& rhs) const;
    
  protected:
    
  private:
    

  // interface from base
  public:
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimscenenoderotaxis_h


