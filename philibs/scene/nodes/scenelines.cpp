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
#include "sceneprog.h"

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
#ifdef TESTINGVS
  camera const* pCam = static_cast< camera const*>(update.mCamPath.getLeaf());

  pni::math::matrix4 viewMat;
  update.mCamPath.calcInverseXform( viewMat );
  
  pni::math::matrix4 modelMat;
  update.mNodePath.calcXform( modelMat );
  
  pni::math::matrix4 projMat = pCam->getProjectionMatrix();
  
  pni::math::matrix4 u_mvpMat;
  
  u_mvpMat.preMult(projMat);
  u_mvpMat.preMult(viewMat);
  u_mvpMat.preMult(modelMat);
  
//  PNIDBGSTR("mvpMat = \n" << mvpMat);
  
  vertIter viter ( getGeomData() );
  size_t const posOff = getGeomData()->mBinding.getValueOffsetBytes(geomData::Position);
  size_t const normOff = getGeomData()->mBinding.getValueOffsetBytes(geomData::Normal);
  
  while ( viter )
  {
      // setup...
    auto rPos = viter.get<vec3 const>(posOff);
    auto rNorm = viter.get<vec3 const>(normOff);

    vec4 a_position ( rPos[ 0 ], rPos[ 1 ], rPos[ 2 ], 1.0f );
    vec4 a_normal ( rNorm[ 0 ], rNorm[ 1 ], rNorm[ 2 ], 0.0f );

      // now we clone the vs

      // get normal in clip... it's still parallel to line segment though
    float lNormLen = a_normal.length();         // local len before matrix mult...
                                                // but meant to be used in screen pixels

    vec4 cSrc = u_mvpMat * a_position;
    vec4 lDst = a_position;
    vec4 lTangent = vec4 ( a_normal / lNormLen ); // same because a_normal[ 4 ] is 0.0

      // HACK: Keep tangent vectors that point toward the eye from projecting
      // through the COP, causing generated points to be on the wrong side of
      // the centerline.  Could be a uniform that defaults to something reasonable.
      // This should probably be proportional to the distance to the near clipping
      // plane.
    lTangent *= 0.01;

      // Offset local position by tangent
    lDst += lTangent;

      // Get tangent end in clip space
    vec4 cDst = u_mvpMat * lDst;
    
      // Save w for points so we can put things back in clip coords later
    float cSrcW  = cSrc[ 3 ];
    float cDstW  = cDst[ 3 ];
    
      // Do perspective divide to get into ndc
    vec4 nSrc = cSrc;
    vec4 nDst = cDst;
    nSrc /= cSrcW;
    nDst /= cDstW;

      // Calc actual normal in ndc
    vec3 nTangent = nDst.xyz () - nSrc.xyz ();
    vec3 nNorm;
    nNorm.cross(nTangent, vec3 { 0.0, 0.0, 1.0 } );
    nNorm.normalize();

      // Make the ndc-based normal the right length in pixels by scaling by
      // the original length divided by forward diff of the pixel size in ndc units.
      // nNorm.xy *= ( lNormLen * u_vpSizeRatio ) / u_vpSize;
    vec2 nNormXY = nNorm.xy();
    vec2 const u_vpSize { 2048.0, 1024.0 }; // Magic numbers!!! But only for diagnostic code.
    nNormXY *= lNormLen;
    nNormXY *= mVpSizeRatio;
    nNormXY /= u_vpSize;
    nNorm.set ( nNormXY[ 0 ], nNormXY[ 1 ], nNorm[ 2 ] );

      // Now add the calculated norm to the point on the line
      // nSrc.xyz += nNorm;
    nSrc.set ( nSrc[ 0 ] + nNorm[ 0 ], nSrc[ 1 ] + nNorm[ 1 ], nSrc[ 2 ] + nNorm[ 2 ], nSrc[ 3 ] );

    // Z offset toward camera to make lines in crotches stand out... but we
    // also do polygon offset in fixed function to make sure fragments move
    // toward the camera.
    // TODO: Scale this based on dz, rather than absolute value.
  // float offset = -pow( 2.0, 8.0 ) / 65536.0;
  // nSrc[ 2 ] += offset;

    ++viter;
  }
#endif // TESTINGVS
}

// ///////////////////////////////////////////////////////////////////

void lines::update ( graphDd::fxUpdate const& update )
{
  if ( mLineData->getDirty() )
  {
    rebuildLines();

    mLineData->clearDirty();
      // GOTCHA: Should not do this during update, which is during scene
      // traversal!
    setGeomBoundsDirty();
  }
  if ( mUniform->getDirty() || mLineStyle.getDirty() )
  {
    rebuildUniform();
//    mUniform->clearDirty(); // For renderer to consume this dirty
    mLineStyle.clearDirty();
  }

#ifdef TESTINGVS
  updateTest(update);
#endif // TESTINGVS
}

// //////////////////////////////////////////////////////////////////

void lines::rebuildLines ()
{
  assert(mLineData);

  lineData const* pLineData = &mLineData->get();
  
  assert(pLineData->mBinding.hasBinding(lineData::Position));
//  assert(mLineData->size());

  if ( pLineData->size() == 0 )
    return; // Early return!!!

  auto curPos = pLineData->begin< vec3 >(lineData::Position);
  auto curColor = pLineData->begin< vec4 >(lineData::Color);
  auto curThickness = pLineData->begin< float >(lineData::Thickness);

  if ( ! getGeomData () )
    setGeomData( new geomData );
  
  geomData* pGeomData = &getGeomData()->op();
  
    // Make sure the geomData matches the number of lines and bindings.
  size_t const points = pLineData->size();
  size_t const prims = pLineData->getIndices().size();
  size_t const segs = points - prims;
  size_t const verts = segs * 2 + prims * 2; // sharing verts, plus the end verts for each prim
  size_t const tris = segs * 2;

    // Start with empty bindings rather than trying to figure out whether they
    // were previously set correctly and doing a delta to current settings.
  geomData::Binding& bindings = pGeomData->mBinding;
  bindings.clear();
  
  bindings.push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, sizeof ( float ), geomData::PositionComponents } );
  bindings.push_back( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, sizeof ( float ), geomData::NormalComponents } );
  bindings.push_back( { CommonAttributeNames[ geomData::TCoord], geomData::TCoord, geomData::DataType_FLOAT, sizeof ( float ), geomData::TCoordComponents, 0 } );

  if ( curColor.good () )
      bindings.push_back( { CommonAttributeNames[ geomData::Color], geomData::Color, geomData::DataType_FLOAT, sizeof ( float ), geomData::ColorComponents } );

  pGeomData->resizeTrisWithCurrentBinding ( verts, tris );
  
  auto pIter = pGeomData->begin<vec3>(geomData::Position);
  auto nIter = pGeomData->begin<vec3>(geomData::Normal);
  auto tIter = pGeomData->begin<vec2>(geomData::TCoord, 0);
  auto cIter = pGeomData->begin<vec4>(geomData::Color);

  geomData::IndexType* pInd = pGeomData->getIndicesPtr();
  size_t vNum = 0;

    // Iterate through all line sigments
  for ( auto& segPoints : pLineData->getIndices() )
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

      *pIter = curPosVec; ++pIter;
      *pIter = curPosVec; ++pIter;

        // Color
      if ( curColor.good () )
      {
        *cIter = *curColor; ++cIter;
        *cIter = *curColor; ++cIter;
        
        ++curColor;
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
      *nIter = diff; ++nIter;
      diff.negate();
      *nIter = diff; ++nIter;
      
        // Fill in UVs... this is probably temporary.  We'll have more
        // sophisticated UVs when we switch to SDF and dash support.
      tIter->set ( lenTotal, -1.0f ); ++tIter;
      tIter->set ( lenTotal, 1.0f ); ++tIter;
      
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
  uniform* pUniform = &mUniform->op();
  pUniform->setName("line uniform");
  lineStyle const& style = mLineStyle.get();

  {
    uniform::binding& binding = pUniform->getBinding("u_thicknessMult");
    binding.set(uniform::binding::Vertex, uniform::binding::Float1);
    binding.setValue(style.mThicknessMult);
  }

  {
    uniform::binding& binding = pUniform->getBinding("u_vpSizeRatio");
    binding.set(uniform::binding::Vertex, uniform::binding::Float2);
    binding.setValue(mVpSizeRatio);
  }

  if ( ( style.mEnableFlags & lineStyle::Tex00 ) == 0 )
  {
    {
      uniform::binding& binding = pUniform->getBinding("u_edgeRange");
      binding.set(uniform::binding::Fragment, uniform::binding::Float2);
      float* pFloats = binding.getFloats();
      pFloats[ 0 ] = style.mEdgeMiddle;
      pFloats[ 1 ] = style.mEdgeRange;
    }

    {
      uniform::binding& binding = pUniform->getBinding("u_dashRange");
      binding.set(uniform::binding::Fragment, uniform::binding::Float4);
      float* pFloats = binding.getFloats();
      pFloats[ 0 ] = style.mDashMiddle;
      pFloats[ 1 ] = style.mDashRange;
      pFloats[ 2 ] = style.mDashPeriod;
      pFloats[ 3 ] = style.mDashPhase;
    }
    
    {
      uniform::binding& binding = pUniform->getBinding("u_dashEnable");
      binding.set(uniform::binding::Fragment, uniform::binding::Int1);
      binding.setValue(style.mEnableFlags);
    }

  }
  else
  {
    prog* pProg = static_cast< prog* > ( getState(state::Prog) );
    
      // Cheeeeezy heuristic to make sure we don't write these flags
      // to the program text too often... should do a better job of this.
    if ( pProg->getDirty() )
    {
      pProg->setFlag("DOTEXTURE00");
      pProg->applyFlags();
    }
  
    {
      uniform::binding& binding = pUniform->getBinding("u_texRange");
      binding.set(uniform::binding::Vertex, uniform::binding::Float3);
      float* pFloats = binding.getFloats();
      pFloats[ 0 ] = style.mTexMiddle;
      pFloats[ 1 ] = style.mTexRange;
      pFloats[ 2 ] = style.mTexUMult;
    }
  }

  {
    uniform::binding& binding = pUniform->getBinding("u_alphaRef");
    binding.set(uniform::binding::Fragment, uniform::binding::Float1);
    float* pFloats = binding.getFloats();
    pFloats[ 0 ] = style.mAlphaRef;
  }
  
  setState(pUniform, state::Uniform00);
}

// ///////////////////////////////////////////////////////////////////

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

