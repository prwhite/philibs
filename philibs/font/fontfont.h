/////////////////////////////////////////////////////////////////////
//
//  class: font
//
/////////////////////////////////////////////////////////////////////

#ifndef fontfontfont_h
#define fontfontfont_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"

#include "imgbase.h"

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
    
    using Id = unsigned short;    // UCS2
    using ImgRef = pni::pstd::autoRef<img::base>;
      
    virtual bool load ( std::string const& fname, std::string const& textureFname );

    std::string const& getTextureFname () const { return mTextureFname; }
  
    void setImage ( img::base* pImg ) { mImg = pImg; }
    img::base* getImage () const { return mImg.get(); }

    float getLineHeight () const { return mLineHeight; }
    float getMaxCharHeight () const { return mMaxCharHeight; }
  
    float getBase () const { return mBase; }
    pni::math::vec4 const& getPadding () const { return mPadding; }

    struct glyph
    {
      Id mId = 0;
      
      pni::math::vec2 mPos;
      pni::math::vec2 mSize;
      pni::math::vec2 mOffset;
      pni::math::vec2 mAdvance;
      pni::math::vec2 mUv00;
      pni::math::vec2 mUv01;
    };
    
    glyph* getGlyph ( Id id );
  
  protected:
    virtual ~font ();
    
    bool parseCommon ( std::ifstream& in );
    bool parseGlyphs ( std::ifstream& in );
    bool parseGlyph ( std::ifstream& in );
    bool loadImg ();

    typedef std::map< Id, glyph > Glyphs;

    std::string mTextureFname;
    Glyphs mGlyphs;
    ImgRef mImg;
    pni::math::vec2 mImgSize;
    float mMaxCharHeight = 0.0f;
    size_t mNumChars = 0;
    float mBase = 0.0f;
    float mLineHeight = 0.0f;
    pni::math::vec4 mPadding; // T, R, B, L ???

  private:


    // interface from pni::pstd::refCount
  public:

  protected:
		virtual void collectRefs ( Refs& refs ) const;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace font 

#endif // fontfontfont_h


