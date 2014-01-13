/////////////////////////////////////////////////////////////////////
//
//    file: scenesprite.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenesprite.h"

#include "sceneconverter.h"

#include "scenetexture.h"

#include <cassert>

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

sprites::sprites () :
  mNumFrames ( 1 ),
  mDoDepthSort ( true ),
  mFrozen ( 0 )
{
    // Init the bindings for the spriteData.
  mData.addBinding ( Position, 3 );
  mData.addBinding ( Size, 2 );
  mData.addBinding ( Flip, 2 );
  mData.addBinding ( Frame, 1 );
  
  assert ( sizeof ( sprite ) == mData.getStride () * 4 );
}

//sprites::~sprites ()
//{
//}
//
//sprites::sprites ( sprites const& rhs )
//{
//}
//
//sprites& sprites::operator= ( sprites const& rhs )
//{
//  return *this;
//}
//
//bool sprites::operator== ( sprites const& rhs ) const
//{
//  return false;
//}

  //mBounds.setIsDirty ( false );

/////////////////////////////////////////////////////////////////////

void sprites::calcRotMat ( graphDd::fxUpdate const& update,
    pni::math::matrix4& mat )
{
    // Get cam -> world matrix to come up with screen-aligned
    // vectors in this node's frame.
    // Invert because the converter gives us world to cam, not
    // cam to world.
  scene::converter conv;
  conv.getWorldToCamMatrix ( mat, update.mCamPath, update.mNodePath );
  mat.invert ();
  
    // Get rid of translate/projection.
    // In some cases losing the projection could be a problem,
    // but in the most likely cases it's no thing at all.
  mat.set3x3Mat ( mat );  // TRICKY: Set to self.  Should be OK.
}

/////////////////////////////////////////////////////////////////////

void sprites::calcXYvecs ( pni::math::matrix4 const& mat )
{
    // The rows represent the transformed i, j and k vectors.
    // So we don't need to do a mult, we just grab them directly.
#ifndef _WIN32
  mat.getRow ( 0, mXvec );
  mat.getRow ( 1, mYvec );
#else
  mat.getRow ( 0, mYvec );
  mat.getRow ( 1, mXvec );
  mYvec *= -1.0f;
#endif
    // Kinda normalize them so that they are unit length in the target frame.
    // We make them half unit length because we will be adding them
    // in each direction from the center of the sprite.
  mXvec /= 2.0f * mXvec.length ();
  mYvec /= 2.0f * mYvec.length ();
}


/////////////////////////////////////////////////////////////////////

void sprites::doDepthSort ( graphDd::fxUpdate const& update,
    Sorters& sorters )
{
    // Calc dist sqr for each position from camPos.
    // camPos is in world frame, so convert to this
    // node's frame.
  pni::math::matrix4 mat;
  update.mNodePath.calcInverseXform ( mat );
  pni::math::vec3 camPos = update.mCamPos;
  camPos.xformPt4 ( camPos, mat );
  
    // Resize vec without calling constructors.
  size_t const end = mData.getElemCount ();
  sorters.reserve ( end );
  
  size_t const stride = mData.getStride ();
  float* pPos = mData.getBindingPtr ( Position );
  for ( SizeType cur = 0;
      cur != end; 
      ++cur, pPos += stride )
  {
    pni::math::vec3* pVec = reinterpret_cast< pni::math::vec3* > ( pPos );
    sorters.push_back ( sorter ( cur,
        camPos.distSqr ( *pVec ) ) );
  }
  
  std::sort ( sorters.begin (), sorters.end () );
}

/////////////////////////////////////////////////////////////////////

void assignVec3ToFloatPtr ( float* pFloat, pni::math::vec3 const& src )
{
  pFloat[ 0 ] = src[ 0 ];
  pFloat[ 1 ] = src[ 1 ];
  pFloat[ 2 ] = src[ 2 ];  
}

void assignUvToFloatPtr ( float* pFloat, pni::math::vec2 const& src )
{
  pFloat[ 0 ] = src[ 0 ];
  pFloat[ 1 ] = src[ 1 ];
}

inline void sprites::addSpriteToGeom ( SizeType dst, SizeType src )
{
  //sprite const& curItem = mItems[ src ];
    // Non-optimal... there's a few extra mults due to
    // calls to getBindingPtr which could be streamlined.
  using namespace pni::math;

  sprite const* const pSprite = reinterpret_cast< sprite const* const > (
      mData.getBindingPtr ( Position, src ) );

    // TRICKY: There's some major weird stuff going on here...
    // our cameras are actually in a space above the transform
    // that gets us z-up.  Thus, they're kinda 90 degrees off.
    // So, we flip x and y in the lines directly below, and then
    // negate one axis to keep things front-facing.  It's lame,
    // but it works and it even makes some sense.
  pni::math::vec3 xVec = mYvec;
  pni::math::vec3 yVec = mXvec;
  
  xVec *= pSprite->mSize[ 0 ];
  yVec *= -pSprite->mSize[ 1 ];

    // TODO: NON-OPTIMAL... initializing this vert iter for every
    // sprite can be amortized.  We could initialize outside of this
    // func but we need to reset it to elem zero every time anyway.
  scene::vertIter viter ( this );
  
    //
  float frameDiv = pSprite->mFlip[ 0 ] * mFrameDiv;
  float x1 = frameDiv * pSprite->mFrame;
  float x2 = x1 + frameDiv;

    // Note: We are flipping x1 and x2 for r vs l because
    // of the -yVec thing above making things mirrored/backfacing.

    // Offset to the beginning of the vert and set up the
    // four unique verts.
    // Possibly NON-OPTIMAL due to using lots of temporaries.
  SizeType valDst = dst * 4;
  viter += valDst;
    // tr = 0
  vec3* posPtr = reinterpret_cast< vec3* > ( &viter  );
  *posPtr = pSprite->mPos;
  *posPtr += xVec;
  *posPtr += yVec;
  vec2* truvPtr = reinterpret_cast< vec2* > ( &viter[ 3 ] );
  truvPtr->set ( x1, pSprite->mFlip[ 1 ] );
  ++viter;
    // br = 1
  posPtr = reinterpret_cast< vec3* > ( &viter  );
  *posPtr = pSprite->mPos;
  *posPtr += xVec;
  *posPtr -= yVec;
  vec2* bruvPtr = reinterpret_cast< vec2* > ( &viter[ 3 ] );
  bruvPtr->set ( x1, 0.0f );
  ++viter;
    // tl = 2
  posPtr = reinterpret_cast< vec3* > ( &viter  );
  *posPtr = pSprite->mPos;
  *posPtr -= xVec;
  *posPtr += yVec;
  vec2* tluvPtr = reinterpret_cast< vec2* > ( &viter[ 3 ] );
  tluvPtr->set ( x2, pSprite->mFlip[ 1 ] );
  ++viter;
    // bl = 3
  posPtr = reinterpret_cast< vec3* > ( &viter  );
  *posPtr = pSprite->mPos;
  *posPtr -= xVec;
  *posPtr -= yVec;
  vec2* bluvPtr = reinterpret_cast< vec2* > ( &viter[ 3 ] );
  bluvPtr->set ( x2, 0.0f );

    // Now index to the verts in non-stripped, tri-strip order.
  geomData::Indices& inds = mGeomData->getIndices ();
  SizeType indDst = dst * 6;
  inds[ indDst++ ] = valDst + 3;
  inds[ indDst++ ] = valDst + 1;
  inds[ indDst++ ] = valDst + 2;
  inds[ indDst++ ] = valDst + 2;
  inds[ indDst++ ] = valDst + 1;
  inds[ indDst   ] = valDst + 0;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//  overrides from geomFx

void sprites::update ( graphDd::fxUpdate const& update )
{
    // TODO: Don't regen if all view params and the
    // positions are unchanged.  Very unlikely this will
    // save us much for most use cases.
    
    // Early bail-out for 'frozen' mode.
  if ( mFrozen > 1 )
    return;

    // Make sure we have a valid geomData;
  if ( ! mGeomData )
    setGeomData ( new geomData );

    // Bail out and clean up if we have no particles.
  if ( mData.getElemCount () == 0 )
  {
    mGeomData->resize ( 0, 0 );
    return; // Early return!!!
  }

    // Calc rotation matrix in the node's frame.  Use converter
    // and values passed in 'update'.
    // Using a member for rotation matrix makes this non-reentrant.
  pni::math::matrix4 rotMat;
  calcRotMat ( update, rotMat );
  calcXYvecs ( rotMat );
    
    // Do depth sort using a temporary index list.
    // TODO: Consider caching the sorters for next time...
    // with the right sort algo, it will be faster as long
    // as there's no cache-miss performance penalty.
  Sorters sorters;
  if ( mDoDepthSort )
    doDepthSort ( update, sorters );
  
    // Set geometry binding.
  mGeomData->bindingsOp().push_back ( { "", geomData::Positions, geomData::DataType_FLOAT, geomData::PositionsComponents } );
  mGeomData->bindingsOp().push_back ( { "", geomData::TCoords00, geomData::DataType_FLOAT, geomData::TCoords00Components } );

    // Unused... why was it here? PRW
//  SizeType stride = mGeomData->getValueStride ();
  
    // Resize geom if sprite array size doesn't match.
    // In the reserve call we are doing 4 unique vert values and
    // 2 tris per sprite (this is a strip-order quad done with tris).
  size_t elemCount = mData.getElemCount ();
  if ( elemCount * 2 != mGeomData->getTriCount () )
    mGeomData->resizeTrisWithCurrentBinding (
        elemCount * 4, elemCount * 2 );

    // Iterate over items creating corresponding sprite tri pairs.    
  if ( mDoDepthSort )
    for ( SizeType num = 0; num < elemCount; ++num )
      addSpriteToGeom ( num, sorters[ num ].mIndex );
  else
    for ( SizeType num = 0; num < elemCount; ++num )
      addSpriteToGeom ( num, num );

    // Set bounds dirty.
  setGeomBoundsDirty ();
  
    // Bump this up only if it's the first time through with frozen on.
  if ( mFrozen == 1 )
    ++mFrozen;
}

void sprites::generateGeomBounds () const
{
    // TODO: Optimize this by iterating over the point array 
    // rather than the geoms like in geomFx::updateBounds.
    // Then, grow the bounds in each dimension by half the maxVal of
    // the largest x/y sprite size.

#define OPTIMIZEDFORSPRITES
#ifdef OPTIMIZEDFORSPRITES
  mBounds.setEmpty ();
  
  spriteData::iterator iter ( const_cast< spriteData* > ( &mData ) );
  
  float maxVal[ 2 ] = { 0.0f, 0.0f };
  
  while ( iter )
  {
    sprite* pSprite = iter.get< sprite > ();
    
    mBounds.extendBy ( pSprite->mPos );
    
    maxVal[ 0 ] = pSprite->mSize[ 0 ] > maxVal[ 0 ] ? pSprite->mSize[ 0 ] : maxVal[ 0 ];
    maxVal[ 1 ] = pSprite->mSize[ 1 ] > maxVal[ 1 ] ? pSprite->mSize[ 1 ] : maxVal[ 1 ];
    
    ++iter;
  }
  
    // This is an approximation at best... so, there are some issues
    // if you use sprites for ui elements and you collide with bounds
    // only, as these bounds might be too small or too big.
  float minAll = maxVal[ 0 ] < maxVal[ 1 ] ? maxVal[ 0 ] : maxVal[ 1 ];
  minAll /= 2.0f; // Only want to grow by half the size in each dimension.
  mBounds.grow ( minAll );
  
  //float maxAll = maxVal[ 0 ] > maxVal[ 1 ] ? maxVal[ 0 ] : maxVal[ 1 ];
  //maxAll /= 2.0f; // Only want to grow by half the size in each dimension.
  //mBounds.grow ( maxAll );
  
#else
  geomFx::generateGeomBounds ();
#endif // OPTIMIZEDFORSPRITES
}

/////////////////////////////////////////////////////////////////////

void sprites::makeSimpleSprite ( pni::math::vec2 const& size, texture* pTex )
{
  this->getSpriteData ().resize ( 1 );
  this->setNumFrames ( 1 );
  this->setDoDepthSort ( false );
  //this->setTravMask ( scene::Draw, 0x00 );
  //this->setTravMask ( scene::Isect, 0x00 );

    // Add a single sprite for the current weapon.
    // Iter must be good for just one sprite.
  spriteData::iterator iter ( &this->getSpriteData () );
  sprites::sprite* pSprite = 
      iter.get< sprites::sprite > ();
  
    // Fix up position and size due to differences in the way the
    // buttons and the sprite textures worked out.
    // Now make the sprite.  
  pSprite->mPos.setIdentity ();
  pSprite->mFrame = 0.0f;
  pSprite->mFlip.set ( 1.0f, 1.0f );
  pSprite->mSize = size;

  //this->setFrozen ( true );  // This is really a reset.
  
    // Setup basic state.
    // Share this texture with weapon select sprites.
  this->setState ( pTex, state::Texture0 );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


