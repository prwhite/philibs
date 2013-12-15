/////////////////////////////////////////////////////////////////////
//
//  class: geomFx
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenegeomfx_h
#define scenescenegeomfx_h

/////////////////////////////////////////////////////////////////////

#include "scenegeom.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class geomFx :
  public geom
{
  public:
    geomFx ();
    //virtual ~geomFx ();
    //geomFx ( geomFx const& rhs );
    //geomFx& operator= ( geomFx const& rhs );
    //bool operator== ( geomFx const& rhs ) const;

    virtual void update ( graphDd::fxUpdate const& update ) = 0;
        
  protected:

  private:


    // interface from geom
  public:
    virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }
    
  protected:
		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenegeomfx_h


