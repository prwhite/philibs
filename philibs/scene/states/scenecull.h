/////////////////////////////////////////////////////////////////////
//
//    class: cull
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenecull_h
#define scenescenecull_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class cull :
  public state
{
  public:
    cull();
    virtual ~cull();
//     cull(cull const& rhs);
//     cull& operator=(cull const& rhs);
//     bool operator==(cull const& rhs) const;
   
   
		/////// define front face direction
		enum FrontFace { ClockWise, CounterClockWise };
		void setFrontFace ( FrontFace faceIn = CounterClockWise ) { mFrontFace = faceIn; }
		FrontFace getFrontFace () const { return mFrontFace; }

		/////// set which face to cull
		enum CullFace { Front, Back };
		void setCullFace ( CullFace cfIn = Back ) { mCullFace = cfIn; }
		CullFace getCullFace () const { return mCullFace; }

    
  protected:
    
  private:
		FrontFace mFrontFace;
		CullFace mCullFace;
    

  // interface from state
  public:
    virtual cull* dup () const { return new cull ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenecull_h


