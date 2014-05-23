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

      /// Calc the rotation matrix that can transform screen space to
      /// the fxUPdate's world/node coordinate system.
      /// This is useful for things like sprites and other screen-aligned
      /// or view-dependent things.
    static void calcCamToWorldRotMat ( graphDd::fxUpdate const& update,
        pni::math::matrix4& mat );

      /// Calc the vectors that represent screen aligned x and y directions.
      /// This is handy for constructing sprites, and other things that
      /// manifest in screen space.
      /// The direction vectors will be half the normalized length in the
      /// node's coordinate frame.
      /// @note The matrix passed in is generally that retrieved from
      /// #calcCamToWorldRotMat.
    static void calcXYHalfVecs ( pni::math::vec3& xvec, pni::math::vec3& yvec,
        pni::math::matrix4 const& mat );


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


