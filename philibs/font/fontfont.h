/////////////////////////////////////////////////////////////////////
//
//  class: font
//
/////////////////////////////////////////////////////////////////////

#ifndef fontfontfont_h
#define fontfontfont_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"

#include "pnivec2.h"
#include "pnivec4.h"

#include <map>
#include <string>

#include <iosfwd>

/////////////////////////////////////////////////////////////////////

namespace font {
    
/////////////////////////////////////////////////////////////////////

class font :
  public pni::pstd::refCount
{
  public:
    font ();
    //font ( font const& rhs );
    //font& operator= ( font const& rhs );
    //bool operator== ( font const& rhs ) const;
    
    typedef unsigned short Id;
      
    virtual bool load ( std::string const& fname, std::string const& textureFname );

    std::string const& getTextureFname () const { return mTextureFname; }

    float mLineHeight;
    struct glyph
    {
      Id mId = 0;
      
      pni::math::vec2 mPos;
      pni::math::vec2 mSize;
      pni::math::vec2 mOffset;
      pni::math::vec2 mAdvance;
    };
    
    glyph* getGlyph ( Id id );
  
  protected:
    virtual ~font ();
    
    bool parseCommon ( std::ifstream& in );
    bool parseGlyphs ( std::ifstream& in );
    bool parseGlyph ( std::ifstream& in );

    typedef std::map< Id, glyph > Glyphs;

    std::string mTextureFname;
    Glyphs mGlyphs;

    size_t mNumChars;

  private:


    // interface from pni::pstd::refCount
  public:

  protected:
		void collectRefs ( Refs& refs ) const
		    {
		    
		    }

};

/////////////////////////////////////////////////////////////////////

} // end of namespace font 

#endif // fontfontfont_h


