/////////////////////////////////////////////////////////////////////
//
//    class: material
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenematerial_h
#define scenescenematerial_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

#include "pnivec4.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class material :
  public state
{
  public:
    material();
    virtual ~material();
//     material(material const& rhs);
//     material& operator=(material const& rhs);
//     bool operator==(material const& rhs) const;


		/////// material properties
		void setDiffuse ( const pni::math::vec4& diffuseIn = pni::math::vec4 ( 0.8f, 0.8f, 0.8f, 1.0f ) ) { mDiffuse = diffuseIn; }
		const pni::math::vec4& getDiffuse () const { return mDiffuse; }

		void setAmbient ( const pni::math::vec4& ambientIn = pni::math::vec4 ( 0.8f, 0.8f, 0.8f, 1.0f ) ) { mAmbient = ambientIn; }
		const pni::math::vec4& getAmbient () const { return mAmbient; }

		void setSpecular ( const pni::math::vec4& specularIn = pni::math::vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) ) { mSpecular = specularIn; }
		const pni::math::vec4& getSpecular () const { return mSpecular; }

		void setEmissive ( const pni::math::vec4& emissiveIn = pni::math::vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ) ) { mEmissive = emissiveIn; }
		const pni::math::vec4& getEmissive () const { return mEmissive; }

		void setShininess ( float shininessIn = 20.0f ) { mShininess = shininessIn; }	// [0,128]
		float getShininess () const { return mShininess; }

		/////// color material mode
// 		enum ColorMode { ModeNone, Emission, Ambient, Diffuse, Specular, AmbientAndDiffuse };
// 		void setColorMode ( ColorMode cmodeIn = ModeNone ) { mCmode = cmodeIn; }
// 		ColorMode getColorMode () const { return mCmode; }

    // Support OES GL_COLOR_MATERIAL glEnable token instead of
    // OpenGL style color material mode that was dropped from OES.
    void setColorMaterialMode ( bool val = false ) { mColorMaterialMode = val; }
    bool getColorMaterialMode () const { return mColorMaterialMode; }

  protected:
    
  private:
		pni::math::vec4 mDiffuse;
		pni::math::vec4 mAmbient;
		pni::math::vec4 mSpecular;
		pni::math::vec4 mEmissive;
		float mShininess;
// 		ColorMode mCmode;
    bool mColorMaterialMode;
    

  // interface from state
  public:
    virtual material* dup () const { return new material ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenematerial_h


