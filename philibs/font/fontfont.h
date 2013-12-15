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
      
    virtual bool load ( std::string const& fname );

    std::string const& getTextureFname () const { return mTextureFname; }

    float mLineHeight;
    float mBase;
    pni::math::vec4 mPadding; // T, R, B, L ???
    //float mPages; // Always 1 for now.

    struct glyph
    {
      glyph () :
        mId ( 0 )
          {}
    
      Id mId;
      
      pni::math::vec2 mPos;
      pni::math::vec2 mSize;
      pni::math::vec2 mOffset;
      pni::math::vec2 mAdvance;
    };
    
    glyph* getGlyph ( Id id );
    float getKern ( Id first, Id second );
    
  protected:
    virtual ~font ();
    
    bool parseCommon ( std::ifstream& in );
    bool parseGlyphs ( std::ifstream& in );
    bool parseGlyph ( std::ifstream& in );
    bool parseKerns ( std::ifstream& in );
    bool parseKern ( std::ifstream& in );

    typedef std::map< Id, glyph > Glyphs;
    typedef std::map< std::pair< Id, Id >, float > Kerns;

    std::string mTextureFname;
    Glyphs mGlyphs;
    Kerns mKerns;

    size_t mNumChars;
    size_t mNumKerns;

  private:


    // interface from pni::pstd::refCount
  public:

  protected:
		void collectRefs ( Refs& refs )
		    {
		    
		    }

};

/////////////////////////////////////////////////////////////////////

} // end of namespace font 

#endif // fontfontfont_h


