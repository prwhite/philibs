/////////////////////////////////////////////////////////////////////
//
//    class: text
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenetext_h
#define scenescenetext_h

/////////////////////////////////////////////////////////////////////

#include "scenegeom.h"

#include "fontfont.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class text :
    public geom
{
    public:
        text();
        //virtual ~text();
        //text(text const& rhs);
        //text& operator=(text const& rhs);
        //bool operator==(text const& rhs) const;
      
      void setFont ( font::font* val );
      font::font* getFont () const { return mFont.get (); }
      
      typedef font::font::Id Id;
      
      struct layout
      {
        layout () :
          mId ( 0 ),
          mScale ( 1.0f, 1.0f ),
          mRot ( 0.0f ),
          mColor ( 1.0f, 1.0f, 1.0f, 1.0f )
            {}
      
        Id mId;
      
        pni::math::vec3 mPos;
        pni::math::vec2 mScale;
        float mRot;               // Rot around y axis.
        pni::math::vec4 mColor;
      };
      
      typedef std::vector< layout > Layouts;
      
      Layouts& getPosVec () { return mPosVec; }
      Layouts const& getPosVec () const { return mPosVec; }
      Layouts& posVecOp () { setDirty (); return mPosVec; }
      
      void update ();
      
    protected:
      typedef pni::pstd::autoRef< font::font > FontRef;
      
      
      FontRef mFont;
      Layouts mPosVec;
      bool mDirty;
      
      void setDirty ( bool val = true );
      void doLayout ();
      void updateTexture ();
      
      
      void collectRefs ( Refs& refs )
          {
            geom::collectRefs ( refs );
            
            refs.push_back ( mFont.get () );
          }
      
    private:
        

	// interface from geom
	public:
		
	protected:
		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenetext_h
