/////////////////////////////////////////////////////////////////////
//
//    class: depth
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenedepth_h
#define scenescenedepth_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

#include "pnivec2.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class depth :
  public state
{
  public:
    depth();
    virtual ~depth();
//     depth(depth const& rhs);
//     depth& operator=(depth const& rhs);
//     bool operator==(depth const& rhs) const;


		enum DepthRangeComponent { Near, Far };
		enum DepthFunc { 
				DepthNever, 
				Less, Equal, LEqual, Greater, NotEqual, GEqual, 
				DepthAlways };

// 		void setDepthRange ( const pni::math::vec2& depthRangeIn = pni::math::vec2 ( 0.0f, 1.0f ) ) 
// 				{ mDepthRange = depthRangeIn; }
// 		const pni::math::vec2& getDepthRange () const { return mDepthRange; }

		void setDepthFunc ( DepthFunc depthFuncIn = Less ) { mDepthFunc = depthFuncIn; }
		DepthFunc getDepthFunc () const { return mDepthFunc; }

		void setDepthMask ( bool depthMaskIn = true ) { mDepthMask = depthMaskIn; }
		bool getDepthMask () const { return mDepthMask; }

    
  protected:
    
  private:
// 		pni::math::vec2 mDepthRange;
		DepthFunc mDepthFunc;
		bool mDepthMask;
    

  // interface from state
  public:
    virtual state* dup () const { return new depth ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenedepth_h


