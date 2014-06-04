/////////////////////////////////////////////////////////////////////
//
//    class: blend
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneblend_h
#define scenesceneblend_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"

#include "scenestatedd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class blend :
  public state
{
  public:
    blend();
    virtual ~blend();
//     blend(blend const& rhs);
//     blend& operator=(blend const& rhs);
//     bool operator==(blend const& rhs) const;


 		enum BlendEquation { Add, Subtract, ReverseSubtract, Min, Max };
 
 		void setBlendEquation ( BlendEquation blendEquationIn = Add ) { mBlendEquation = blendEquationIn; }
 		BlendEquation getBlendEquation () const { return mBlendEquation; }

		/////// blend func
		enum SrcFunc { 
				SrcZero, 
				SrcOne, 
				SrcDstColor, 
				SrcOneMinusDstColor, 
				SrcAlpha, 
				SrcOneMinusSrcAlpha, 
				SrcDstAlpha, 
				SrcOneMinusDstAlpha, 
				SrcAlphaSaturate };
		enum DstFunc { 
				DstZero, 
				DstOne, 
				DstSrcColor, 
				DstOneMinusSrcColor, 
				DstSrcAlpha, 
				DstOneMinusSrcAlpha, 
				DstAlpha, 
				DstOneMinusDstAlpha };

		void setBlendFunc ( SrcFunc srcIn = SrcAlpha, DstFunc dstIn = DstOneMinusSrcAlpha ) { mSrcFunc = srcIn; mDstFunc = dstIn; }
		void getBlendFunc ( SrcFunc& srcOut, DstFunc& dstOut ) const { srcOut = mSrcFunc; dstOut = mDstFunc; }

		
		//////// alpha func
		enum AlphaFunc {
			AlphaNever,
			Less,
			Equal,
			LEqual,
			Greater ,
			NotEqual,
			GEqual,
			AlphaAlways 			
		};

		void setAlphaFunc ( AlphaFunc aFuncIn = AlphaAlways ) { mAlphaFunc = aFuncIn; }
		AlphaFunc getAlphaFunc () const { return mAlphaFunc; }

		void setAlphaRef ( float valIn = 2.0f / 255.0f ) { mAlphaRef = valIn; }
		float getAlphaRef () const { return mAlphaRef; }


  protected:
    
  private:
 		BlendEquation mBlendEquation;
		SrcFunc mSrcFunc;
		DstFunc mDstFunc;
		AlphaFunc mAlphaFunc;
		float mAlphaRef;
    

  // interface from state
  public:
    virtual blend* dup () const { return new blend ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneblend_h


