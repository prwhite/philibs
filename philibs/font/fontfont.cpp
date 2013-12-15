/////////////////////////////////////////////////////////////////////
//
//    file: fontfont.cpp
//
/////////////////////////////////////////////////////////////////////

#include "fontfont.h"

#include <fstream>

/////////////////////////////////////////////////////////////////////

namespace font {
    
/////////////////////////////////////////////////////////////////////

font::font () :
  mLineHeight ( 0.0f ),
  mBase ( 0.0f ),
  mNumChars ( 0 )
{
  // TODO
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

bool font::load ( std::string const& fname )
{
  std::ifstream in ( fname.c_str () );
  
  if ( in.good () )
  {
    parseCommon ( in );
    parseGlyphs ( in );
    parseKerns ( in );    
    
    return in.good ();  // Make sure we didn't run out of data.
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
  
  size_t const BufSize = 256;
  char buf[ BufSize ];
  
  in.getline ( buf, BufSize );

    // Read global params.
  float size, bold, italic, unicode, stretchH, smooth, aa, 
      pad00, pad01, pad02, pad03, spacing00, spacing01, outline;

  if ( sscanf ( buf, "info face=%*s size=%f bold=%f italic=%f charset=%*s unicode=%f stretchH=%f smooth=%f aa=%f padding=%f,%f,%f,%f spacing=%f,%f outline=%f",
      &size, &bold, &italic, &unicode, &stretchH, &smooth, &aa, 
      &pad00, &pad01, &pad02, &pad03, &spacing00, &spacing01, &outline )
    < 14 )
      return false;

  mPadding[ 0 ] = pad00;
  mPadding[ 1 ] = pad01;
  mPadding[ 2 ] = pad02;
  mPadding[ 3 ] = pad03;
  
  in.getline ( buf, BufSize );

    // Read common params.
  float scaleW, scaleH, pages, packed, encoded;
  if ( sscanf ( buf, "common lineHeight=%f base=%f scaleW=%f scaleH=%f pages=%f packed=%f encoded=%f", 
      &mLineHeight, &mBase,
      &scaleW, &scaleH, &pages, &packed, &encoded )
    < 7 )
      return false;
      
    // Read first page... or in our case, the only page.
  in.getline ( buf, BufSize );
  
  char textureFname[ BufSize ];
  int page;
  
  if ( sscanf ( buf, "page id=%d file=\"%[^\"]s\"", &page, textureFname )
    < 2 )
      return false;
  mTextureFname = textureFname;
  
  in.getline ( buf, BufSize );
  
  if ( sscanf ( buf, "chars count=%ld", &mNumChars )
    < 1 )
      return false;

  return true;
}

/////////////////////////////////////////////////////////////////////

bool font::parseGlyphs ( std::ifstream& in )
{
  bool retVal = true;

  for ( size_t num = 0; num < mNumChars; ++num )
    retVal &= parseGlyph ( in );
  
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

    // By-value copy is expensive... but simple.
    // TODO: Profile this to see if it's a hotspot.
  mGlyphs[ cur.mId ] = cur;
 
  return true;
}

/////////////////////////////////////////////////////////////////////

bool font::parseKerns ( std::ifstream& in )
{
  bool retVal = true;

  size_t const BufSize = 256;
  char buf[ BufSize ];

  in.getline ( buf, BufSize );
  
  if ( sscanf ( buf, "kernings count=%ld", &mNumKerns ) 
    < 1 )
      return false;
  
  for ( size_t num = 0; num < mNumKerns; ++num )
    retVal &= parseKern ( in );
  
  return retVal;
}

/////////////////////////////////////////////////////////////////////

bool font::parseKern ( std::ifstream& in )
{
  size_t const BufSize = 256;
  char buf[ BufSize ];

  in.getline ( buf, BufSize );

  Kerns::key_type key;
  float val = 0.0f;

  if ( sscanf ( buf, "kerning first=%hd second=%hd amount=%f", &key.first, &key.second, &val )
    < 3 )
      return false;
  
  mKerns[ key ] = val;
  
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

/////////////////////////////////////////////////////////////////////

float font::getKern ( Id first, Id second )
{
  Kerns::iterator found = mKerns.find ( Kerns::key_type ( first, second ) );
  
  if ( found != mKerns.end () )
    return found->second;
  else
    return 0;
}

/////////////////////////////////////////////////////////////////////
//  overrides from pni::pstd::refCount


/////////////////////////////////////////////////////////////////////


} // end of namespace font 


