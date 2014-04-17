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
//    The main ramification of this change is far less of the global
//    and common info is stored in the SDF font description.  Also,
//    kerning is not stored in this format either.
/////////////////////////////////////////////////////////////////////

#include "fontfont.h"

#include <fstream>

/////////////////////////////////////////////////////////////////////

namespace font {
    
/////////////////////////////////////////////////////////////////////

font::font () :
  mLineHeight ( 0.0f ),
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

bool font::load ( std::string const& fname, std::string const& textureFname )
{
  std::ifstream in ( fname.c_str () );
  mTextureFname = textureFname;
  
  if ( in.good () )
  {
    parseCommon ( in );
    parseGlyphs ( in );
    
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
  
  char face[ BufSize ];

  if ( sscanf ( buf, "info face=%s", face )
    < 1 )
      return false;
  
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

    // Cheesy way to approximate line height since this SDF format doesn't
    // retain it.
  if ( cur.mSize[ 1 ] > mLineHeight )
    mLineHeight = cur.mSize[ 1 ];

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

/////////////////////////////////////////////////////////////////////
//  overrides from pni::pstd::refCount


/////////////////////////////////////////////////////////////////////


} // end of namespace font 


