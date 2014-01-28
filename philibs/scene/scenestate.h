/////////////////////////////////////////////////////////////////////
//
//    class: state
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenestate_h
#define scenescenestate_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include "pnirefcount.h"
#include "sceneattr.h"

#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////

namespace scene {
  class stateDd;
  
/////////////////////////////////////////////////////////////////////

class state :
    public pni::pstd::refCount,
    public travDataContainer
{
  public:
    
      // Massive conceptual encapsulation breakage...
      // rather than have some PITA id allocation scheme
      // or rely on RTTI.
      // Every derived state must have an Id entry here.
      enum Id
      {
//         AlphaMult,  //
        Blend,
// //         CameraPath,
// //         Clear,
//         Clip,       //
//         ColorMask,  //
//         ColorMult,  //
//         CubeMap,    //
        Cull,
        Depth,
//         Dsp,        //
//         Fog,        //
        Lighting,
        LightPath,
//         LineStyle,  //
        Material,
// //         Normalize,
//         PointStyle, //
//         PolygonMode,   // Not in OES
//         PolygonOffset,   //
        Prog,
//         Scissor,    //
//         Stencil,    //
//        TexEnv0, // TexEnv1 - TexEnv0 must == 1, etc.
//        TexEnv1,
//         TexGen0,
//         TexGen1,
        Texture00,
        Texture01,
        Texture02,
        Texture03,
        Texture04,
        Texture05,
        Texture06,
        Texture07,
        Texture08,
        Texture09,
        Texture10,
        Texture11,
        Texture12,
        Texture13,
        Texture14,
        Texture15,
        
        
//        TextureXform0,
//        TextureXform1,
// //         Viewport,

          // For shader-defined uniform values.  Apps need to ensure
          // their geomData attributes, programs, and uniforms all
          // line up.
        Uniform00,
        Uniform01,
        Uniform02,
        Uniform03,
        Uniform04,
        Uniform05,
        Uniform06,
        Uniform07,
        Uniform08,
        Uniform09,


        StateCount,
        NumTexUnits = Texture15 - Prog
      };
  
    static_assert ( NumTexUnits == 16, "someone changed the number of texture units calculation" );

    state();
    virtual ~state();
//     state(state const& rhs);
//      state& operator=(state const& rhs);
//       bool operator==(state const& rhs) const;
        
    // State framework methods
    virtual void accept ( stateDd* pDd ) const = 0;
    virtual state* dup () const = 0;

//     virtual Id getId () const = 0;
    
    // Concrete state methods.
    void setName ( std::string const& str ) { mName = str; }
    std::string const& getName () const { return mName; }

    void setEnable ( bool val ) { mEnable = val; }
    bool getEnable () const { return mEnable; }
    
    void setOverride ( bool val ) { mOverride = val; }
    bool getOverride () const { return mOverride; }

  protected:

    std::string mName;
    bool mEnable;
    bool mOverride;
    
  private:

  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );  
};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenestate_h


