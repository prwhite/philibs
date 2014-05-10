/////////////////////////////////////////////////////////////////////
//
//    file: scenetext.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenetext.h"
#include "scenetexture.h"
#include "scenecommon.h"
#include "imgbase.h"

#include <cassert>
#include <locale>
#include <codecvt>

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

//text::text()
//{
//    
//}
//
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

/////////////////////////////////////////////////////////////////////

void text::doLayout ()
{
//  img::base* pImg = mFont->getImage();
//  img::base::Dim size[ 3 ];
//  pImg->getSize(size[ 0 ], size[ 1 ], size[ 2 ]);

    // Make sure we have a geom data thingie.
  geomData* pData = getGeomData();
  if ( ! pData )
  {
    pData = new geomData;
    pData->attributesOp().push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );
    pData->attributesOp().push_back ( { CommonAttributeNames[ geomData::TCoord00], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );
    setGeomData(pData);
  }

  std::wstring_convert<std::codecvt_utf8<char16_t>,char16_t> cvt;
  std::u16string uTxt = cvt.from_bytes(mTxt);

  size_t numGoodChars = uTxt.size();

  pData->resizeTrisWithCurrentAttributes(numGoodChars * 4, numGoodChars * 2);

  float xPos = 0.0f;
  float yPos = 0.0f;
  size_t curVert = 0;

  float* pAttr = pData->getAttributePtr(geomData::Position);
  geomData::IndexType* pInd = pData->getIndicesPtr();

  for ( auto iter : uTxt )
  {
    if ( font::font::glyph* pGlyph = mFont->getGlyph(iter) )
    {
      float xLeft = pGlyph->mOffset[ 0 ] + xPos;
      float xRight = pGlyph->mOffset[ 0 ] + xPos + pGlyph->mSize[ 0 ];
//      float yTop = mFont->getMaxCharHeight() - pGlyph->mOffset[ 1 ] + yPos + pGlyph->mSize[ 1 ];
//      float yBot = mFont->getMaxCharHeight() - pGlyph->mOffset[ 1 ] + yPos + pGlyph->mSize[ 1 ] * 2.0f;
      float yTop = mFont->getLineHeight() - pGlyph->mOffset[ 1 ] + yPos - pGlyph->mSize[ 1 ];
      float yBot = mFont->getLineHeight() - pGlyph->mOffset[ 1 ] + yPos;

      float invLineHeight = 1.0f / mFont->getLineHeight();

        // Normalize on line height
      xLeft *= invLineHeight;
      xRight *= invLineHeight;
      yTop *= invLineHeight;
      yBot *= invLineHeight;

      pAttr[ 0 ] = xLeft;
      pAttr[ 1 ] = yBot;
      pAttr[ 2 ] = 0.0f;
      pAttr[ 3 ] = pGlyph->mUv00[ 0 ];
      pAttr[ 4 ] = pGlyph->mUv00[ 1 ];
      pAttr += 5;
      
      pAttr[ 0 ] = xRight;
      pAttr[ 1 ] = yBot;
      pAttr[ 2 ] = 0.0f;
      pAttr[ 3 ] = pGlyph->mUv01[ 0 ];
      pAttr[ 4 ] = pGlyph->mUv00[ 1 ];
      pAttr += 5;
      
      pAttr[ 0 ] = xLeft;
      pAttr[ 1 ] = yTop;
      pAttr[ 2 ] = 0.0f;
      pAttr[ 3 ] = pGlyph->mUv00[ 0 ];
      pAttr[ 4 ] = pGlyph->mUv01[ 1 ];
      pAttr += 5;

      pAttr[ 0 ] = xRight;
      pAttr[ 1 ] = yTop;
      pAttr[ 2 ] = 0.0f;
      pAttr[ 3 ] = pGlyph->mUv01[ 0 ];
      pAttr[ 4 ] = pGlyph->mUv01[ 1 ];
      pAttr += 5;

#define CCW
#ifndef CCW
      pInd[ 0 ] = curVert;
      pInd[ 1 ] = curVert + 1;
      pInd[ 2 ] = curVert + 2;
      pInd[ 3 ] = curVert + 1;
      pInd[ 4 ] = curVert + 3;
      pInd[ 5 ] = curVert + 2;
#else
      pInd[ 0 ] = curVert;
      pInd[ 1 ] = curVert + 2;
      pInd[ 2 ] = curVert + 1;
      pInd[ 3 ] = curVert + 1;
      pInd[ 4 ] = curVert + 2;
      pInd[ 5 ] = curVert + 3;
#endif // CCW
      pInd += 6;
      
      curVert += 4;
      xPos += pGlyph->mAdvance[ 0 ];
    }
    else
    {
      PNIDBGSTR("Could not get glyph for char " << iter);
      --numGoodChars;
    }
  }
  
    // Resize the attribute and index arrays if some of the characters did not
    // generate geometry because the glyphs were missing.
  pData->resizeTrisWithCurrentAttributes(numGoodChars * 4, numGoodChars * 2);
}

/////////////////////////////////////////////////////////////////////

void text::collectRefs ( Refs& refs ) const
{
  geom::collectRefs ( refs );
  
  refs.push_back ( mFont.get () );
}


/////////////////////////////////////////////////////////////////////
// overrides from geom


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


