/////////////////////////////////////////////////////////////////////
//
//    class: texEnv
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenetexenv_h
#define scenescenetexenv_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class texEnv :
  public state
{
  public:
    texEnv();
    virtual ~texEnv();
//     texEnv(texEnv const& rhs);
//     texEnv& operator=(texEnv const& rhs);
//     bool operator==(texEnv const& rhs) const;


		// TODO register combiners

		enum Mode { Modulate, Decal, Replace, Blend, Add, Combine };
		enum CombineRGB { RGBReplace, RGBModulate, RGBAdd, RGBAddSigned, 
				RGBInterp, RGBSubtract, RGBDot3RGB, RGBDot3RGBA };
		enum CombineAlpha { AReplace, AModulate, AAdd, AAddSigned,
				AInterp, ASubtract };
		enum Source { Arg0, Arg1, Arg2 };
		enum SourceType { Texture, Constant, PrimaryColor, Previous };
		enum RGBOp { RGBSrcColor, RGBOneMinusSrcColor, RGBSrcAlpha, RGBOneMinusSrcAlpha };
		enum AlphaOp { ASrcAlph, AOneMinusSrcAlpha };

		// env mode
		void setMode ( Mode modeIn = Modulate ) { mMode = modeIn; }
    Mode getMode () const { return mMode; }

		///// combine functions
    void setCombineRGB ( CombineRGB combineRGBIn = RGBModulate ) { mCombineRGB = combineRGBIn; }
		CombineRGB getCombineRGB () const { return mCombineRGB; }

		void setCombineAlpha ( CombineAlpha combineAlphaIn = AModulate ) { mCombineAlpha = combineAlphaIn; }
		CombineAlpha getCombineAlpha () const { return mCombineAlpha; }

		// combine sources
		void setRgbSourceType ( Source srcIn, SourceType rgbSourceTypeIn = Texture ) { mRgbSourceType[ srcIn ] = rgbSourceTypeIn; }
		SourceType getRgbSourceType ( Source srcIn ) const { return mRgbSourceType[ srcIn ]; }

		void setAlphaSourceType ( Source srcIn, SourceType alphaSourceTypeIn = Texture ) { mAlphaSourceType[ srcIn ] = alphaSourceTypeIn; }
		SourceType getAlphaSourceType ( Source srcIn ) const { return mAlphaSourceType[ srcIn ]; }

		// combine ops
		void setRgbOp ( Source srcIn, RGBOp rgbOpIn = RGBSrcColor ) { mRgbOp[ srcIn ] = rgbOpIn; }
		RGBOp getRgbOp ( Source srcIn ) const { return mRgbOp[ srcIn ]; }

		void setAlphaOp ( Source srcIn, AlphaOp alphaOpIn = ASrcAlph ) { mAlphaOp[ srcIn ] = alphaOpIn; }
		AlphaOp getAlphaOp ( Source srcIn ) const { return mAlphaOp[ srcIn ]; }

		///// blend/constant color
		void setBlendColor ( const pni::math::vec4& blendColorIn = pni::math::vec4 ( 0.0f, 0.0f, 0.0f, 0.0f ) ) { mBlendColor = blendColorIn; }
		const pni::math::vec4& getBlendColor () const { return mBlendColor; }

    
  protected:
    
  private:
    Mode mMode;
		CombineRGB mCombineRGB;
		CombineAlpha mCombineAlpha;
		SourceType mRgbSourceType[ 3 ];
		SourceType mAlphaSourceType[ 3 ];
		RGBOp mRgbOp[ 3 ];
		AlphaOp mAlphaOp[ 3 ];
		pni::math::vec4 mBlendColor;
    

  // interface from state
  public:
    virtual state* dup () const { return new texEnv ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenetexenv_h


