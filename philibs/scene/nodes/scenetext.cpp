/////////////////////////////////////////////////////////////////////
//
//    file: scenetext.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenetext.h"
#include "scenetexture.h"

#include <cassert>

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

text::text() :
  mDirty ( true )
{
    
}

//text::~text()
//{
//    
//}
//
//text::text(text const& rhs)
//{
//    
//}
//
//text& text::operator=(text const& rhs)
//{
//    
//    return *this;
//}
//
//bool text::operator==(text const& rhs) const
//{
//    
//    return false;
//}

void text::setFont ( font::font* val )
{
  mFont = val;
  setDirty ();
  updateTexture ();
}

/////////////////////////////////////////////////////////////////////

void text::setDirty ( bool val )
{
  mDirty = val;
}

/////////////////////////////////////////////////////////////////////

void text::update ()
{
  if ( ! mDirty )
    return;       // Early return!!!
  
  doLayout ();
}

/////////////////////////////////////////////////////////////////////

void text::doLayout ()
{
  
}

/////////////////////////////////////////////////////////////////////

void text::updateTexture ()
{
  // We will leave most of this to the app because we don't
  // currently have a texture factory, and we don't want to 
  // hard-code a loader into this implementation.
  
  texture* pTex = static_cast< texture* > ( getState ( state::Texture0 ) );

  assert ( pTex );
  
  img::base* pImg = pTex->getImage ();
  
  assert ( pImg );
  
  assert ( pImg->getFormat () == img::base::Alpha8 );
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// overrides from geom


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


