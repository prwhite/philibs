/////////////////////////////////////////////////////////////////////
//
//    file: fontfont.cpp
//
//    Was originally made to load AngelCode Bitmap font generator
//    files.
//
//    Was then adapted to use lonesock's SDF implementation from here:
//    http://www.gamedev.net/topic/491938-signed-distance-bitmap-font-tool/
//
//    Then, was updated to support Hiero font generator.  Still not getting
//    kerns, but get most of the global and common params now.  Also, packing
//    algo not as good in Hiero as lonesock.
//
//    Not sure after last change if lonesock loads correctly, but should,
//    and also a possibility that lonesock character positions don't match
//    Hiero positions.
//
/////////////////////////////////////////////////////////////////////

#include "fontfont.h"

#include "imgfactory.h"

#include "pnidbg.h"

#include <fstream>

/////////////////////////////////////////////////////////////////////

namespace font {
    
/////////////////////////////////////////////////////////////////////

font::font ()
{

}

font::~font ()
{
  // TODO
}

//font::font ( font const& rhs )
//{
//  // TODO
//}
//
//font& font::operator= ( font const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool font::operator== ( font const& rhs ) const
//{
//  // TODO
//  return false;
//}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

bool font::load ( std::string const& fname, std::string const& textureFname )
{
  std::ifstream in ( fname.c_str () );
  mTextureFname = textureFname;
  
  if ( in.good () )
  {
    if ( loadImg() )
    {
      bool retParse = parseCommon ( in );
      
      return in.good () && retParse;
    }
    else
      return false;
  }
  else
    return false;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

bool font::parseCommon ( std::ifstream& in )
{
    // Note, this doesn't match the data model of the
    // font file... we collapse all common and page 
    // processing into one... because we won't support
    // multi-page fonts.
  
  bool ret = true;
  
  size_t const BufSize = 256;
  char buf[ BufSize ];
  
  while ( ret )
  {
    in.getline ( buf, BufSize );

    std::string line ( buf );

    if ( line.find ( "info face=" ) != std::string::npos )
    {
      float size, bold, italic, unicode, stretchH, smooth, aa,
          pad00, pad01, pad02, pad03, spacing00, spacing01, outline;
      
      char face[ BufSize ] = { 0 };

      size_t scanned = sscanf ( buf, "info face=%s size=%f bold=%f italic=%f charset=%*s unicode=%f stretchH=%f smooth=%f aa=%f padding=%f,%f,%f,%f spacing=%f,%f outline=%f",
          face, &size, &bold, &italic, &unicode, &stretchH, &smooth, &aa,
          &pad00, &pad01, &pad02, &pad03, &spacing00, &spacing01, &outline );
      if ( scanned == 1 )
      {
          // Assuming we got face name from lonesock format.
          // Not doing anything with facename right now.
      }
      else if ( scanned == 14 )
      {
          // Only grabbing padding right now.
        mPadding[ 0 ] = pad00;
        mPadding[ 1 ] = pad01;
        mPadding[ 2 ] = pad02;
        mPadding[ 3 ] = pad03;
      }
      else
        ret &= false;
    }
    else if ( line.find ( "common lineHeight=" ) != std::string::npos )
    {
        // encoded doesn't show up in Hiero output
      float scaleW, scaleH, pages, packed, encoded;
      if ( sscanf ( buf, "common lineHeight=%f base=%f scaleW=%f scaleH=%f pages=%f packed=%f encoded=%f", 
          &mLineHeight, &mBase,
          &scaleW, &scaleH, &pages, &packed, &encoded )
        < 6 )
      {
        ret &= false;
      }
    }
    else if ( line.find ( "page id=" ) != std::string::npos  )
    {
      // Don't give a f*ck about this currently.
    }
    else if ( line.find ( "chars count=" ) != std::string::npos  )
    {
      if ( sscanf ( buf, "chars count=%ld", &mNumChars )
        < 1 )
        ret &= false;
      else
      {
        return parseGlyphs ( in );
      }
    }
    else
    {
      PNIDBGSTR("unknown line in font file" << line);
      return false;
    }
  }
  
  return ret;
}

/////////////////////////////////////////////////////////////////////

bool font::parseGlyphs ( std::ifstream& in )
{
  bool retVal = true;
  mMaxCharHeight = 0.0f;

  for ( size_t num = 0; num < mNumChars; ++num )
    retVal &= parseGlyph ( in );
  
    // Do we have a valid line height?  If not, make it a small multiple of
    // the max character height we determined.
  if ( mLineHeight == 0.0f )
    mLineHeight = 1.5f * mMaxCharHeight;  // Magic number!!!
  
  return retVal;
}

/////////////////////////////////////////////////////////////////////

bool font::parseGlyph ( std::ifstream& in )
{
  glyph cur;
  
  size_t const BufSize = 256;
  char buf[ BufSize ];
  
  in.getline ( buf, BufSize );
  
  size_t page, chnl;
  
  if ( sscanf ( buf, "char id=%hd x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%f page=%ld chnl=%ld",
      &cur.mId,
      &cur.mPos[ 0 ], &cur.mPos[ 1 ],
      &cur.mSize[ 0 ], &cur.mSize[ 1 ],
      &cur.mOffset[ 0 ], &cur.mOffset[ 1 ],
      &cur.mAdvance[ 0 ], // No mAdvance[ 1 ] in format.
      &page, &chnl )
    < 10 )
      return false;

    // Cache UVs so we don't calc these every time we create text geometry.
  cur.mUv00 = cur.mPos / mImgSize;
  cur.mUv01 = ( cur.mPos + cur.mSize ) / mImgSize;
  
    // Cheesy way to approximate line height since this SDF format doesn't
    // retain it.
  if ( cur.mSize[ 1 ] > mMaxCharHeight )
    mMaxCharHeight = cur.mSize[ 1 ];

    // By-value copy is expensive... but simple.
    // TODO: Profile this to see if it's a hotspot.
  mGlyphs[ cur.mId ] = cur;
 
  return true;
}

/////////////////////////////////////////////////////////////////////

font::glyph* font::getGlyph ( Id id )
{
    // Do we need a more interesting policy here?
    // If we do handle missing glyphs, it will be
    // in layout routines.
  Glyphs::iterator found = mGlyphs.find ( id );
  if ( found != mGlyphs.end () )
    return &found->second;
  else
    return 0;
}

bool font::loadImg ()
{
  if ( (mImg = img::factory::getInstance().loadSync(mTextureFname)) )
  {
    img::base::Dim imgSize[ 3 ];
    mImg->getSize( imgSize[ 0 ], imgSize[ 1 ], imgSize[ 2 ] );
    
    mImgSize.set ( imgSize[ 0 ], imgSize[ 1 ] );  // Ignoring img pitch for now...
                                                  // /should/ be equal to width.
    return true;
  }
  else
    return false;
}

/////////////////////////////////////////////////////////////////////
//  overrides from pni::pstd::refCount

void font::collectRefs ( Refs& refs ) const
{
  refs.push_back(mImg.get());
}


/////////////////////////////////////////////////////////////////////


} // end of namespace font 


