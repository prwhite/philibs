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

      enum Stage { Vertex, Fragment, NumStages };

      void setProgStr ( Stage stage, std::string const& str ) { mProg[ stage ] = str; setDirty (); }
      std::string const& getProgStr ( Stage stage ) const { return mProg[ stage ]; }

      void setDirty ( bool val = true ) { mDirty = val; }
      bool getDirty () const { return mDirty; }
      void clearDirty () const { mDirty = false; }

      void setDefaultProgs ();

  protected:
    
  private:
    std::string mProg[ NumStages ] = {};

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


