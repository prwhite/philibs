// ////////////////////////////////////////////////////////////////////////////
//
//  class: lines
//
// ////////////////////////////////////////////////////////////////////////////

#include "scenelines.h"
#include "scenecommon.h"
#include "pnivec2.h"
#include "pnivec3.h"
#include "pnivec4.h"

#include <cassert>

// ///////////////////////////////////////////////////////////////////

using namespace pni::math;

namespace scene {
    
// ///////////////////////////////////////////////////////////////////

void lines::update ( graphDd::fxUpdate const& update )
{
  mLineData.clearDirty();
  mUniform.clearDirty();
}

void lines::rebuildLines()
{
  assert(mLineData);
  assert(mLineData->mBinding.hasBinding(lineData::Position));
  assert(mLineData->size());

  auto curPos = mLineData->begin< vec3 >(lineData::Position);
  auto curColor = mLineData->begin< vec4 >(lineData::Color);
  auto curThickness = mLineData->begin< float >(lineData::Thickness);

  if ( ! getGeomData() )
    setGeomData( new geomData );
  
  geomData* pGeomData = geometryOp();
  
    // Make sure the geomData matches the number of lines and bindings.
  size_t points = mLineData->size();
  size_t segs = points - mLineData->getIndices().size();
  size_t verts = segs * 4;
  size_t tris = segs * 2;

  geomData::Attributes attrs;
  attrs.push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );
  attrs.push_back( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );
  attrs.push_back( { CommonAttributeNames[ geomData::TCoord00], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );

  if ( curColor.good () )
      attrs.push_back( { CommonAttributeNames[ geomData::Color], geomData::Color, geomData::DataType_FLOAT, geomData::ColorComponents } );
  
  pGeomData->setAttributes( attrs );

  pGeomData->resizeTrisWithCurrentAttributes( verts, tris );
  
  size_t stride = pGeomData->getAttributes().getValueStride();
  float* pPos = pGeomData->getAttributePtr(geomData::Position);
  float* pNorm = pGeomData->getAttributePtr(geomData::Normal);
  float* pUv = pGeomData->getAttributePtr(geomData::TCoord00);
  float* pColor = pGeomData->getAttributePtr(geomData::Color);
  geomData::IndexType* pInd = pGeomData->getIndicesPtr();
  size_t vNum = 0;

    // Iterate through all line sigments
  for ( auto segNum : mLineData->getIndices() )
  {
    size_t end = segNum - 1;

      // Iterate through all points in each segment
    for ( size_t cur = 0; cur < end; ++cur )
    {
        // Position
      vec3 const& curPosVec = *curPos;
      ++curPos;
      vec3 const& nextPosVec = *curPos;

      curPosVec.copyToArray( pPos ); pPos += stride;
      curPosVec.copyToArray( pPos ); pPos += stride;
      nextPosVec.copyToArray( pPos ); pPos += stride;
      nextPosVec.copyToArray( pPos ); pPos += stride;
      
      if ( curColor.good () )
      {
        vec4 const& curColorVec = *curColor;
        ++curColor;
        vec4 const& nextColorVec = *curColor;
        
        curColorVec.copyToArray( pColor ); pColor += stride;
        curColorVec.copyToArray( pColor ); pColor += stride;
        nextColorVec.copyToArray( pColor ); pColor += stride;
        nextColorVec.copyToArray( pColor ); pColor += stride;
      }
      
        // Default thickness, and then fill in real thickness values if they
        // are spec'd per-point.
      float curThicknessVal = 1.0f;
      float nextThicknessVal = 1.0f;
      
      if ( curThickness.good () )
      {
        float const& curThicknessFloat = *curThickness;
        ++curThickness;
        float const& nextThicknessFloat = *curThickness;
        
        curThicknessVal = curThicknessFloat;
        nextThicknessVal = nextThicknessFloat;
      }

        // Set normals that are actually the vec from cur to next, multiplied
        // by the point's thickness.
        // TODO: Make next points diff based on next next point, except at the
        // end of a line seg where it will be equal to next (as it is now).
      vec3 diff = nextPosVec;
      diff -= curPosVec;
      
      float len = diff.length();
      diff /= len;  // normalize
      diff *= curThicknessVal;
      
        // Set cur point's normals, inverting in order to change which side
        // of the line it will describe in the vertex shader.
      diff.copyToArray( pNorm ); pNorm += stride;
      diff.negate();
      diff.copyToArray( pNorm ); pNorm += stride;

        // Set next point's normals, inverting in order to change which side
        // of the line it will describe in the vertex shader.
      diff *= nextThicknessVal / curThicknessVal;
      diff.negate();
      diff.copyToArray( pNorm ); pNorm += stride;
      diff.negate();
      diff.copyToArray( pNorm ); pNorm += stride;
      
        // Fill in UVs... this is probably temporary.  We'll have more
        // sophisticated UVs when we switch to SDF and dash support.
      vec2 uv;
      uv.set ( 0.0f, 0.0f );
      uv.copyToArray( pUv ); pUv += stride;
      uv.set ( 0.0f, 1.0f );
      uv.copyToArray( pUv ); pUv += stride;
      uv.set ( len,  0.0f );
      uv.copyToArray( pUv ); pUv += stride;
      uv.set ( len,  1.0f );
      uv.copyToArray( pUv ); pUv += stride;
      
        // Set indices and increment vert count
        // Verts are laid out like so:
        // 1 --- 3
        // | '-. |
        // 0 --- 2
      *pInd++ = vNum + 0;
      *pInd++ = vNum + 2;
      *pInd++ = vNum + 1;
      *pInd++ = vNum + 1;
      *pInd++ = vNum + 2;
      *pInd++ = vNum + 3;
      vNum += 4;
    }
    
    ++curPos;
    if ( curColor.good () ) ++curColor;
    if ( curThickness.good () ) ++curThickness;
  }
}

void lines::rebuildUniform ()
{
  uniform::binding& binding = mUniform.op()->bindingOp("u_vpSizeTarget");
  binding.set(uniform::binding::Vertex, uniform::binding::Float2);
  float* pFloats = binding.getFloats();
//  pFloats[ 0 ] = mVpSizeTarget[ 0 ];
//  pFloats[ 1 ] = mVpSizeTarget[ 1 ];
}

void lines:: generateGeomBounds () const
{
#define OPTIMIZEDFORLINES
#ifdef OPTIMIZEDFORLINES
    // Note that the bounds only represents the points, not including the
    // thickness of the lines.  This means things hitting screen edge
    // could cull early, but in practice this will be rare and visually
    // minor.
  assert(mLineData);
  assert(mLineData->mBinding.hasBinding(lineData::Position));
  assert(mLineData->size());

  mBounds.setEmpty();

  auto cur = mLineData->begin< vec3 >(lineData::Position);
  auto end = mLineData->end< vec3 >(lineData::Position);
  
  while ( cur != end )
  {
    mBounds.extendBy(*cur);
    
    ++cur;
  }

#else
  geomFx::generateGeomBounds();
#endif // OPTIMIZEDFORLINES
}

// ///////////////////////////////////////////////////////////////////


} // end of namespace scene 

