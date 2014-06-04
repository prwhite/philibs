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

void lines::updateTest ( graphDd::fxUpdate const& update )
{
  // Used to figure out if vertex shader is doing something unexpected...
  // so far, looks like "yes".  The vs is calculating normw as 0 sometimes
  // while the cpu version is not.  Matrices and vertex points have been
  // verified to match in this simulation of the vs.  It all goes wrong in
  // the vs when transforming norm (after norm += pos) by the mvpMat.
  // Ah... so the problem was that the norm added to pos was out of range
  // in some way for the vs after tranformation by mvpMat.  Fixed it by
  // normalizing norm after storing its length away for later.  It's a scary
  // fix as it may indicate there will be other range/precision issues, or that
  // this created a situation where things were behind the near view plane
  // causing a singularity to be hit or some such.  Will continue to watch for
  // this.
//#define TESTINGVS
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
      // MEGA: problem in vs was that norm needed to be... uh... normalized
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

void lines::update ( graphDd::fxUpdate const& update )
{
  mLineData.clearDirty();
  mUniform.clearDirty();

#ifdef TESTINGVS
  updateTest(update);
#endif // TESTINGVS
}

void lines::rebuildLines ()
{
  assert(mLineData);
  assert(mLineData->mBinding.hasBinding(lineData::Position));
//  assert(mLineData->size());

  if ( mLineData->size() == 0 )
    return; // Early return!!!

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
    auto lastThickness = curThickness;
    lastThickness += segPoints - 1;

    float lenTotal = 0.0f;   // start before 0 to give room for start cap

      // TODO: Need to stretch first and last segments by cur and last
      // thickness so that end caps (depending on style) are centered on
      // the point, rather than starting at the point.  For the time
      // being, our line is not the right length, but we use lenRatio
      // to make sure the end UV matches what would be the proper length.
      // When we do make the geometry longer, we can drop use of lenRatio
      // below.  With lenRatio, our caps will be a little compressed.
      // TODO: Need to make lenFull, and the start/end thicknesses uniforms
      // because they're needed in the fs.
//    float lenTotal = - *curThickness;   // start before 0 to give room for start cap
//    float const lenFull = preCalcLength();
//    float const lenWithCaps = lenFull + *curThickness + *lastThickness;
//    float const lenRatio = lenWithCaps / lenFull;
    

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
//        assert(0);
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
      uv.set ( lenTotal, -1.0f );
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

float lines::preCalcLength()
{
  auto cur = mLineData->begin< pni::math::vec3 >( lineData::Position );
  auto end = mLineData->end< pni::math::vec3 >( lineData::Position );
  
  float ret = 0.0;
  
  if ( ! ( cur == end ) )
  {
    pni::math::vec3 lastPos = *cur;
    ++cur;
    
    while ( cur != end )
    {
      auto const& curPos = *cur;
      ret += curPos.dist(lastPos);
      lastPos = curPos;
    
      ++cur;
    }
  }
  
  return ret;
}

  // From lines::style
//      pni::math::vec2 mEdgeRange { 0.5f, 0.15f }; // [0] = middle alpha, [1] = +- range
//      pni::math::vec2 mDashRange { 0.3f, 0.05f }; // [0] = middle alpha, [1] = +- range
//      float mAlphaRef { 0.1f };                   // Discard threshold
//      uint32_t mDashEnable { 0 };                 // Turn dashes on/off


void lines::rebuildUniform ()
{
  {
    uniform::binding& binding = mUniform.op()->bindingOp("u_vpSizeRatio");
    binding.set(uniform::binding::Vertex, uniform::binding::Float2);
    float* pFloats = binding.getFloats();
    pFloats[ 0 ] = mVpSizeRatio[ 0 ];
    pFloats[ 1 ] = mVpSizeRatio[ 1 ];
  }

  {
    uniform::binding& binding = mUniform.op()->bindingOp("u_edgeRange");
    binding.set(uniform::binding::Fragment, uniform::binding::Float2);
    float* pFloats = binding.getFloats();
    pFloats[ 0 ] = mStyle->mEdgeMiddle;
    pFloats[ 1 ] = mStyle->mEdgeRange;
  }

  {
    uniform::binding& binding = mUniform.op()->bindingOp("u_dashRange");
    binding.set(uniform::binding::Fragment, uniform::binding::Float4);
    float* pFloats = binding.getFloats();
    pFloats[ 0 ] = mStyle->mDashMiddle;
    pFloats[ 1 ] = mStyle->mDashRange;
    pFloats[ 2 ] = mStyle->mDashPeriod;
    pFloats[ 3 ] = mStyle->mDashPhase;
  }

  {
    uniform::binding& binding = mUniform.op()->bindingOp("u_alphaRef");
    binding.set(uniform::binding::Fragment, uniform::binding::Float1);
    float* pFloats = binding.getFloats();
    pFloats[ 0 ] = mStyle->mAlphaRef;
  }
  
  {
    uniform::binding& binding = mUniform.op()->bindingOp("u_dashEnable");
    binding.set(uniform::binding::Fragment, uniform::binding::Int1);
    int* pInts = binding.getInts();
    pInts[ 0 ] = mStyle->mEnableFlags;
  }
  
  setState(mUniform.get(), state::Uniform00);
}


void lines::rebuildStyle ()
{
  mUniform.setDirty();
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
//  assert(mLineData->size());

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

