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
        //text();
        //virtual ~text();
        //text(text const& rhs);
        //text& operator=(text const& rhs);
        //bool operator==(text const& rhs) const;
      
      void setFont ( font::font* val ) { mFont = val;  doLayout(); }
      font::font* getFont () const { return mFont.get (); }
  
      void setText ( std::string const& txt ) { mTxt = txt; doLayout(); }
      std::string const& getText () const { return mTxt; }
  

    protected:
      using Id = font::font::Id;
      using FontRef = pni::pstd::autoRef< font::font >;
  
      FontRef mFont;
      std::string mTxt;
  
      virtual void doLayout ();
  
    /// @group interface from refCount
	public:
      virtual void collectRefs ( Refs& refs ) const;
      
    private:
        

    // @group interface from geom
	public:
		
	protected:
		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenetext_h
