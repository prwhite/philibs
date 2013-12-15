/////////////////////////////////////////////////////////////////////
//
//    class: light
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenelight_h
#define scenescenelight_h

/////////////////////////////////////////////////////////////////////

#include "scenenode.h"
#include "scenegraphdd.h"

#include "pnivec4.h"
#include "pnivec3.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class light :
  public node
{
  public:
    light();
//     virtual ~light();
//     light(light const& rhs);
//     light& operator=(light const& rhs);
//     bool operator==(light const& rhs) const;


		enum Type { Directional, Point, Spot };

		void setType ( Type typeIn = Directional ) { mType = typeIn; }
		Type getType () const { return mType; }

		void setAmbient ( pni::math::vec4 ambientIn = pni::math::vec4 ( 0.2f, 0.2f, 0.2f, 1.0f ) ) { mAmbient = ambientIn; }
		const pni::math::vec4& getAmbient () const { return mAmbient; }

		void setDiffuse ( pni::math::vec4 diffuseIn = pni::math::vec4 ( 0.8f, 0.8f, 0.8f, 1.0f ) ) { mDiffuse = diffuseIn; }
		const pni::math::vec4& getDiffuse () const { return mDiffuse; }

		void setSpecular ( pni::math::vec4 specularIn = pni::math::vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) ) { mSpecular = specularIn; }
		const pni::math::vec4& getSpecular () const { return mSpecular; }

		// if the fourth component is 1.0 it's a local light
		// the position of a directional light is ignored... duh
		void setPosition ( pni::math::vec4 positionIn = pni::math::vec4 ( 0.0f, 0.0f, 0.0f, 0.0f ) ) { mPosition = positionIn; }
		const pni::math::vec4& getPosition () const { return mPosition; }

		void setDirection ( pni::math::vec3 directionIn = pni::math::vec3 ( 0.0f, 1.0f, 0.0f ) ) { mDirection = directionIn; }
		const pni::math::vec3& getDirection () const { return mDirection; }

		// only applies to spots
		void setExponent ( float exponentIn = 1.0f ) { mExponent = exponentIn; }		// [0,128]
		float getExponent () const { return mExponent; }

		// only applies to spots
		void setCutoffAngle ( float cutoffAngleIn = 180.0f ) { mCutoffAngle = cutoffAngleIn; }	// [0,90],180
		float getCutoffAngle () const { return mCutoffAngle; }

		enum Attenuation { Constant, Linear, Quadratic };
		// default is no attenuation
		void setAttenuation ( pni::math::vec3 attenuationIn = pni::math::vec3 ( 1.0f, 0.0f, 0.0f ) ) { mAttenuation = attenuationIn; }
		const pni::math::vec3& getAttenuation () const { return mAttenuation; }
		
	protected:
		
	private:
		Type mType;
		pni::math::vec4 mAmbient;
		pni::math::vec4 mDiffuse;
		pni::math::vec4 mSpecular;
		pni::math::vec4 mPosition;
		pni::math::vec3 mDirection;
		float mExponent;
		float mCutoffAngle;
		pni::math::vec3 mAttenuation;
 
    

  // interface from node
  public:
    virtual node* dup () const { return new light ( *this ); }
    virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenelight_h


