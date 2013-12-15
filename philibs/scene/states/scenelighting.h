/////////////////////////////////////////////////////////////////////
//
//    class: lighting
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenelighting_h
#define scenescenelighting_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

#include "pnivec4.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class lighting :
  public state
{
  public:
    lighting();
    virtual ~lighting();
    lighting(lighting const& rhs);
    lighting& operator=(lighting const& rhs);
    bool operator==(lighting const& rhs) const;
    

		void setAmbient ( const pni::math::vec4& ambientIn = pni::math::vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ) ) { mAmbient = ambientIn; }
		const pni::math::vec4& getAmbient () const { return mAmbient; }
		
// 		void setLocalViewer ( bool localViewerIn = true ) { mLocalViewer = localViewerIn; }
// 		bool getLocalViewer () const { return mLocalViewer; }

// 		void setSeparateSpecular ( bool separateSpecularIn = true ) { mSeparateSpecular = separateSpecularIn; }
// 		bool getSeparateSpecular () const { return mSeparateSpecular; }


  protected:
    
  private:
 		pni::math::vec4 mAmbient;
// 		bool mLocalViewer;
// 		bool mSeparateSpecular;
   

  // interface from state
  public:
    virtual state* dup () const { return new lighting ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenelighting_h


