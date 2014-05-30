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

//#define TESTINGVS
#ifdef TESTINGVS
#include "pnimatrix4.h"
#include "scenecamera.h"
#include "pnimathstream.h"
#endif // TESTINGVS

// ///////////////////////////////////////////////////////////////////

using namespace pni::math;

namespace scene {
    
// ///////////////////////////////////////////////////////////////////

void lines::update ( graphDd::fxUpdate const& update )
{
  mLineData.clearDirty();
  mUniform.clearDirty();

  // Used to figure out if vertex shader is doing something unexpected...
  // so far, looks like "yes".  The vs is calculating normw as 0 sometimes
  // while the cpu version is not.  Matrices and vertex points have been
  // verified to match in this simulation of the vs.  It all goes wrong in
  // the vs when transforming norm (after norm += pos) by the mvpMat.
#ifdef TESTINGVS
  camera const* pCam = static_cast< camera const*>(update.mCamPath.getLeaf());

  pni::math::matrix4 viewMat;
  update.mCamPath.calcInverseXform( viewMat );
  
  pni::math::matrix4 modelMat;
  update.mNodePath.calcXform( modelMat );
  
  pni::math::matrix4 projMat = pCam->getProjectionMatrix();
  
  pni::math::matrix4 mvpMat;
  
  mvpMat.preMult(projMat);
  mvpMat.preMult(viewMat);
  mvpMat.preMult(modelMat);
  
//  PNIDBGSTR("mvpMat = \n" << mvpMat);
  
  vertIter viter ( getGeomData() );
  size_t posOff = mGeomData->getAttributes().getValueOffset(geomData::Position);
  size_t normOff = mGeomData->getAttributes().getValueOffset(geomData::Normal);
  
  while ( viter )
  {
    float const* pPos = viter ( posOff );
    float const* pNorm = viter ( normOff );

    vec4 pos ( pPos[ 0 ], pPos[ 1 ], pPos[ 2 ], 1.0f );
    vec4 norm ( pNorm[ 0 ], pNorm[ 1 ], pNorm[ 2 ], 0.0f );

      // norm is a position, not a vector now
    norm += pos;

      // now go in clip space
    pos.xformPt(pos, mvpMat);
    norm.xformPt(norm, mvpMat);
    
      // now go in ndc
    float posw = pos[ 3 ];
    float normw = norm[ 3 ];
    
    pos /= posw;
    norm /= normw;
    
    if ( Trait::equal(normw, 0.0f, 0.1f ) )
    {
      PNIDBGSTR( "how is normw 0-ish?" );
    }
    ++viter;
  }
#endif // TESTINGVS
}

void lines::rebuildLines ()
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
  size_t prims = mLineData->getIndices().size();
  size_t segs = points - prims;
  size_t verts = segs * 2 + prims * 2; // sharing verts, plus the end verts for each prim
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
  for ( auto segPoints : mLineData->getIndices() )
  {
    size_t end = segPoints;
    
    vec3 lastPosVec;
    float lenTotal = 0.0f;

      // Iterate through all points in each segment
    for ( size_t cur = 0; cur < end; ++cur )
    {
      bool first = ( cur == 0 );
      bool last = ( cur == ( end - 1 ) );
    
        // Position... we need last and next for normal mitering below.
      vec3 const& curPosVec = *curPos;
      ++curPos;
      if ( first )
        lastPosVec = curPosVec;         // Repeat first pos as last
        // Repeat cur pos as next if at end
      vec3 const& nextPosVec = last ? curPosVec : *curPos;

      curPosVec.copyToArray( pPos ); pPos += stride;
      curPosVec.copyToArray( pPos ); pPos += stride;
      
        // Color
      if ( curColor.good () )
      {
        vec4 const& curColorVec = *curColor;
        ++curColor;
        curColorVec.copyToArray( pColor ); pColor += stride;
        curColorVec.copyToArray( pColor ); pColor += stride;
      }
      
        // Default thickness, and then fill in real thickness values if they
        // are spec'd per-point.
      float curThicknessVal = 1.0f;
      if ( curThickness.good () )
      {
        float const& curThicknessFloat = *curThickness;
        ++curThickness;
        curThicknessVal = curThicknessFloat;
      }

        // Set normals that are actually the vec from cur to next, multiplied
        // by the point's thickness.
        // TODO: Make next points diff based on next next point, except at the
        // end of a line seg where it will be equal to next (as it is now).
      vec3 lastDiff ( vec3::NoInit );
      vec3 nextDiff ( vec3::NoInit );
      vec3 tangent ( vec3::NoInit );
      float len = 0.0f;

        // TODO: Optimize
      if ( first && last )
      {
        assert(0);
      }
      else if ( first )
      {
        nextDiff = nextPosVec - curPosVec;
        len = nextDiff.length();
        nextDiff /= len;
        lastDiff = nextDiff;
        tangent = nextDiff;
      }
      else if ( last )
      {
        lastDiff = curPosVec - lastPosVec;
        lastDiff.normalize();
        nextDiff = lastDiff;
        tangent = lastDiff;
      }
      else
      {
        lastDiff = curPosVec - lastPosVec;
        lastDiff.normalize();
        nextDiff = nextPosVec - curPosVec;
        len = nextDiff.length();
        nextDiff /= len;
        tangent = ( nextDiff + lastDiff ) / 2.0f;
      }
      
        // From https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader
        // Now figure out the right length for the miter edge
      float newLen = curThicknessVal / tangent.dot ( lastDiff );
      vec3 diff =  tangent * newLen;
      
        // Set cur point's normals, inverting in order to change which side
        // of the line it will describe in the vertex shader.
      diff.copyToArray( pNorm ); pNorm += stride;
      diff.negate();
      diff.copyToArray( pNorm ); pNorm += stride;
      
        // Fill in UVs... this is probably temporary.  We'll have more
        // sophisticated UVs when we switch to SDF and dash support.
      vec2 uv;
      uv.set ( lenTotal, 0.0f );
      uv.copyToArray( pUv ); pUv += stride;
      uv.set ( lenTotal, 1.0f );
      uv.copyToArray( pUv ); pUv += stride;
      
        // Set indices and increment vert count
        // Verts are laid out like so:
        // 1 --- 3 --- 5
        // | '-. | '-. |
        // 0 --- 2 --- 4
      if ( ! first )
      {
        *pInd++ = vNum + 0;
        *pInd++ = vNum + 2;
        *pInd++ = vNum + 1;
        *pInd++ = vNum + 1;
        *pInd++ = vNum + 2;
        *pInd++ = vNum + 3;
        vNum += 2;
      }
      
      lastPosVec = curPosVec;
      lenTotal += len;
    }
    vNum += 2;
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

