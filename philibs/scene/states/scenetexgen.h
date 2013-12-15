/////////////////////////////////////////////////////////////////////
//
//    class: texGen
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenetexgen_h
#define scenescenetexgen_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"
#include "pniplane.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class texGen :
  public state
{
  public:
    texGen();
    virtual ~texGen();
//     texGen(texGen const& rhs);
//     texGen& operator=(texGen const& rhs);
//     bool operator==(texGen const& rhs) const;
    

		enum Coord { SCoord, TCoord, RCoord, QCoord, NumCoords };
		enum Function { Disable, ObjectLinear, EyeLinear, Spherical, Reflection, Normal };

		void setFunction ( Coord which, Function functionIn = Disable ) { mFunctions[ which ] = functionIn; }
		Function getFunction ( Coord which ) const { return mFunctions[ which ]; }

    void setPlane ( Coord which, const pni::math::plane& planeIn = pni::math::plane ( 1.0f, 0.0f, 0.0f, 0.0f ) ) { mPlanes[ which ] = planeIn; }
		const pni::math::plane& getPlane ( Coord which ) const { return mPlanes[ which ]; }

		// convenience function for texture projection
		void setMatrix ( const pni::math::matrix4& mat4, Function funcIn );	// func must be ObjectLinear or EyeLinear


  protected:
    
  private:
		Function mFunctions[ NumCoords ];
		pni::math::plane mPlanes[ NumCoords ];
    

  // interface from state
  public:
    virtual state* dup () const { return new texGen ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenetexgen_h


