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

#include <vector>

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

      enum Stage { Vertex, Fragment, NumStages };

        /// Set the GLSL program string for the indicated stage.
      void setProgStr ( Stage stage, std::string const& str ) { mProg[ stage ] = str; setDirty (); }
      std::string const& getProgStr ( Stage stage ) const { return mProg[ stage ]; }

      void setDirty ( bool val = true ) { mDirty = val; }
      bool getDirty () const { return mDirty; }
      void clearDirty () const { mDirty = false; }

        /// Calls setProgStr with basic rendering progs.
        /// @note Will call equiv of applyFlags at end of setting vert/frag prog text.
      void setDefaultProgs ();
  
      enum Flag
      {
        Tex,
        Uv,
        UvReflection,

        Sampler2D,
        SamplerCube,
        
        Color,
        Normal,
        
        DefaultLight, // TODO
      };
  
        /// Set an enable flag for one of the shader's functions.  If the
        /// flag corresponds to a texture or light unit, specify the unit
        /// in the second argument.
        /// @note You must call setDefaultProgs or applyFlags after changing flags.
      void setFlag ( Flag flag, uint32_t unit = -1 ); // { mFlagVec.push_back( { flag, unit } ); }
  
        /// Set a custom define string flag.  Can combine this with custom
        /// program text to do more than the default programs/flags.
        /// @param flag String will be turned into "#define <flag>[<unit>]"
        /// and prepended to program when #applyFlags is called.
      void setFlag ( std::string const& flag, uint32_t unit = -1 );
      void resetFlags () { mFlagVec.clear(); }
  
        /// Apply currently set flags... i.e., apply defines determined by flags
        /// to currently set program text.
      void applyFlags ();

  protected:
    
  private:
    struct flagPair
    {
      std::string mFlag;
      uint32_t mUnit;
    };
  
    typedef std::vector< flagPair > FlagVec;
  
    std::string mProg[ NumStages ] = {};
    FlagVec mFlagVec;
    mutable bool mDirty;

  // interface from state
  public:
    virtual state* dup () const { return new prog ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneprog_h


