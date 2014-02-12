/////////////////////////////////////////////////////////////////////
//
//    file: ldterrain.cpp
//
/////////////////////////////////////////////////////////////////////

#define PSTDPROFDISABLE
#define  PNIPSTDLOGDISABLE
#include "pnilog.h"

#include "scenehterrain.h"

#include "scenetexenv.h"
#include "scenematerial.h"
#include "scenelighting.h"

#include "sceneisectdd.h"

#include "scenecommon.h"

#include "imgalgo.h"

#include "pnivec2.h"
#include "pnirand.h"

#include <cassert>

  // TEMP
#include "pnitimerprof.h"

/////////////////////////////////////////////////////////////////////

using namespace pni::math;

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

namespace {
 
void initImageWithSize ( img::base* pImg, 
    img::base::Dim width, img::base::Dim height, img::base::Format format )
{
    // pitch == width because we're dealing with grayscale imagery.
  pImg->setSize ( width, height, width );
  pImg->setFormat ( format );
  
  img::base::buffer* pBuffer = new img::base::buffer;
  pBuffer->resize ( width * height * pImg->calcFormatStride ( format ), 0xff );
  
  pImg->mBuffers.push_back ( pBuffer );
}

} // end anonymous namespace

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

hterrain::hterrain () :
  mSqrt64 ( 128.0f ),
  mHeightScale ( 0.725f )
{
  setBoundsMode ( Static | Partition );
}

hterrain::~hterrain ()
{
  // TODO
}

//hterrain::hterrain ( hterrain const& rhs )
//{
//  // TODO
//}
//
//hterrain& hterrain::operator= ( hterrain const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool hterrain::operator== ( hterrain const& rhs ) const
//{
//  // TODO
//  return false;
//}

void hterrain::initDiffImg ( img::base* pImg )
{
  mDiffImg = pImg;
  
    // Texture unit 0.
  mDiffTex = new scene::texture;
  mDiffTex->setImage ( mDiffImg.get () );
  mDiffTex->setMagFilter ( scene::texture::MagLinear );
  if ( mDiffImg->mBuffers.size () > 1 )
    mDiffTex->setMinFilter ( scene::texture::MinLinearMipNearest );
  else
    mDiffTex->setMinFilter ( scene::texture::MinLinear );
  setState ( mDiffTex.get (), scene::state::Texture00 );
  
  Dim width, height, pitch;
  mDiffImg->getSize ( width, height, pitch );
  mDiffImg->getSize ( mDiffSize[ 0 ], mDiffSize[ 1 ], mDiffSize[ 2 ] );  
  
    // Texture unit 1.
  if ( mFxImg )
  {
  
  }
  else
  {
    img::base* pFxImg = new img::base;
    mDiffImg->setName ( "generated hterrain fx image" );
    initImageWithSize ( pFxImg, width, height, img::base::Gray8  );
    mFxImg = pFxImg;
  }
  
  mFxTex = new scene::texture;
  mFxTex->setImage ( mFxImg.get () );
  mFxTex->setMagFilter ( scene::texture::MagLinear );
  mFxTex->setMinFilter ( scene::texture::MinLinear ); // Too bad.
  setState ( mFxTex.get (), scene::state::Texture01 );
  
  scene::texEnv* pEnv = new scene::texEnv;
  pEnv->setEnable ( true );

    // TODO: PRW PNIGLES1REMOVED
#ifdef PNIGLES1REMOVED
  setState ( pEnv, scene::state::TexEnv00 );
  setState ( pEnv, scene::state::TexEnv01 );
#endif // PNIGLES1REMOVED

  scene::material* pMat = new scene::material;
  pMat->setDiffuse ( pni::math::vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) );
  pMat->setEnable ( true );
  setState ( pMat, scene::state::Material );
  
  scene::lighting* pLighting = new scene::lighting;
  pLighting->setEnable ( true );
  setState ( pLighting, scene::state::Lighting );
}

/////////////////////////////////////////////////////////////////////

void hterrain::initHeightImg ( img::base* pImg )
{
  mHeightImg = pImg;

  //Dim width, height, pitch;
  //mHeightImg->getSize ( width, height, pitch );
  mHeightImg->getSize ( mHeightSize[ 0 ], mHeightSize[ 1 ], mHeightSize[ 2 ] );

  mDz = calcHeightDz ();

  rebuildGeom ( 1, 1, mHeightSize[ 0 ] - 1, mHeightSize[ 1 ] - 1 );
}

/////////////////////////////////////////////////////////////////////

void hterrain::initFxImg ( img::base* pImg )
{
  mFxImg = pImg;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void hterrain::convertToHeightFrame ( pni::math::vec3& where, float& radius )
{
    // Update inv matrix.
  mInvMatrix = mMatrix;
  mInvMatrix.invert ();

    // Transform 'where'.
  where.xformPt4 ( where, mInvMatrix );
  where[ 2 ] /= mDz;
  
    // Transform radius vec to get new length of radius in img frame.
    // GOTCHA: TRICKY: Ignore y and z axis.
  pni::math::vec3 radiusVec ( radius, 0.0f, 0.0f );
  radiusVec.xformVec4 ( radiusVec, mInvMatrix );
  radius = radiusVec[ 0 ];
}

void hterrain::convertToDiffuseFrame ( pni::math::vec3& where, float& radius )
{
  convertToHeightFrame ( where, radius );
  
  Dim width, height, pitch;
  mDiffImg->getSize ( width, height, pitch );
  
  float ratio = ( float ) width;
  
  mHeightImg->getSize ( width, height, pitch );
  
  ratio /= width;
  
  where *= ratio;
  radius *= ratio;
}

/////////////////////////////////////////////////////////////////////

bool hterrain::isPointInImg ( img::base* pImg, int xPos, int yPos )
{
  Dim width, height, pitch;
  pImg->getSize ( width, height, pitch );

  return ( xPos >= 0 || xPos < ( int ) width 
      || yPos >= 0 || yPos < ( int ) height );
}

void hterrain::clipPointToImg ( img::base* pImg, int& xPos, int& yPos )
{
  Dim width, height, pitch;
  pImg->getSize ( width, height, pitch );

  xPos = xPos >= 0 ? xPos : 0;
  xPos = xPos < ( int ) width ? xPos : ( int ) width - 1;
  
  yPos = yPos >= 0 ? yPos : 0;
  yPos = yPos < ( int ) height ? yPos : ( int ) height - 1;
}

/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

struct tVert
{
  pni::math::vec3 mPos;
  pni::math::vec3 mNorm;
  pni::math::vec2 mUv0;
  pni::math::vec2 mUv1;
};

struct cellArgs
{
  typedef img::base::BufferType Btype;

  img::base::Dim mXpos;
  img::base::Dim mYpos;
  Btype* x0y0;
  Btype* xpy0;
  Btype* x0yp;
  Btype* xny0;
  Btype* x0yn;
  float mUvDx;
  float mUvDy;
  float mDz;
  
  tVert* mVert;
  
  void incrX ()
      {
        ++mXpos;
        ++x0y0;
        ++xpy0;
        ++x0yp;
        ++xny0;
        ++x0yn;
        ++mVert;
      }

};

/////////////////////////////////////////////////////////////////////

inline void lutNormalize ( pni::math::vec3& vec )
{
PSTDPROFCALL(static pstd::timerProf rg ( "lutNormalize", 24 * 24 * 2 );
rg.start ();)

#define USEFASTINVSQRT
//#define USELUT
#ifdef USELUT
  // 256 / 20 ~= 13
  // 256 / 40 ~= 6
  // 256 / 80 ~= 3
  static hterrain::sqrtLut sqrtLut ( 32.0f, true );
  
  float dot = vec.dot ( vec );
  vec *= sqrtLut[ dot ];  // sqrtLut is alrady in 1/length form.
#else
  #ifdef USEFASTINVSQRT
    //float dot = vec.dot ( vec );
    //vec *= pni::math::Trait::fastInvSqrt ( dot, 2 );
    fastNormalize ( vec, 3 );

//float normalLen = vec.length ();
//float fastLen = fastLength ( vec, 3 );
//
//printf ( "err = %f\n", fastLen / normalLen );

  #else

    vec.normalize ();
  
  #endif // USEFASTINVSQRT
#endif

PSTDPROFCALL(rg.stop ();)
}

/////////////////////////////////////////////////////////////////////

  //     2 3    5
  //    x0yp - x1y1
  //      |  \\  |
  //    x0y0 - xpy0
  //      0    1 4

void addVert ( cellArgs const& args )
{
  args.mVert[ 0 ].mPos.set ( 
      ( float ) args.mXpos, 
      ( float ) args.mYpos, 
      ( float ) *args.x0y0 * args.mDz );

    // Get positions of right and forward verts from this vert.
  pni::math::vec3 v0 ( 
      ( float ) args.mXpos + 1, 
      ( float ) args.mYpos, 
      ( float ) *args.xpy0 * args.mDz );
  pni::math::vec3 v1 ( 
      ( float ) args.mXpos, 
      ( float ) args.mYpos + 1, 
      ( float ) *args.x0yp * args.mDz );
  
    // TODO: Optimize out the normalize calls if possible.
  
    // Turn into vecs from mVert[ 0 ].
  v0 -= args.mVert[ 0 ].mPos;
  v1 -= args.mVert[ 0 ].mPos;
  lutNormalize ( v0 );
  lutNormalize ( v1 );
  
    // Now cross to get normal.
  args.mVert[ 0 ].mNorm.cross ( v0, v1 );
  lutNormalize ( args.mVert[ 0 ].mNorm );

    // Do again for left and back verts from this vert.
  v0.set ( 
      ( float ) ( args.mXpos - 1 ), 
      ( float ) args.mYpos, 
      ( float ) *args.xny0 * args.mDz );
  v1.set ( 
      ( float ) args.mXpos, 
      ( float ) ( args.mYpos - 1 ), 
      ( float ) *args.x0yn * args.mDz );
  lutNormalize ( v0 );
  lutNormalize ( v1 );

    // Result is normalized.  
  v0.cross ( v0, v1  );
  
  args.mVert[ 0 ].mNorm += v0;    // Add together.
  args.mVert[ 0 ].mNorm /= 2.0f;  // Average.
  lutNormalize ( args.mVert[ 0 ].mNorm );
  
    // Now set up UVs.
  args.mVert[ 0 ].mUv0.set ( 
      args.mUvDx * args.mXpos, args.mUvDy * args.mYpos );
  args.mVert[ 0 ].mUv1 = args.mVert[ 0 ].mUv0;
}

/////////////////////////////////////////////////////////////////////

float hterrain::calcHeightDz ()
{
    // To set hterrain height scale relative to x/y size.
  return mHeightScale / ( ( mHeightSize[ 0 ] + mHeightSize[ 1 ] ) / 2.0f );  // Average of width & height.
}

/////////////////////////////////////////////////////////////////////

  // TODO: Rebuild geom with additional dirty rect parameter so that
  // doCrater can just update a subset of the geom.
void hterrain::rebuildGeom ( Dim xStart, Dim yStart, Dim xEnd, Dim yEnd )
{
  using namespace scene;

  Dim width, height, pitch;
  mHeightImg->getSize ( width, height, pitch );
  
  Dim const xBase = 1;
  Dim const yBase = 1;
  Dim const xLimit = width - 1;
  Dim const yLimit = height - 1;

  if ( xStart < xBase ) xStart = xBase;
  if ( yStart < yBase ) yStart = yBase;
  if ( xEnd > xLimit ) xEnd = xLimit;
  if ( yEnd > yLimit ) yEnd = yLimit;

  Dim valuePitch = xLimit - xBase;

    // We are trimming off the edges because we can't get
    // real data for crossing to get normals, etc.
    // TODO: Make these parameters to the func so that
    // we can rebuild subsets of the hterrain.
  //Dim const xStart = 1;
  //Dim const yStart = 1;
  //Dim const xEnd = width - 1;
  //Dim const yEnd = height - 1;

PNIPSTDLOG

  if ( ! mGeomData )
  {
    setGeomData ( new geomData );

    geomData::Attributes& attributes = mGeomData->attributesOp();

    attributes.push_back ( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );
    attributes.push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );
    attributes.push_back ( { CommonAttributeNames[ geomData::TCoord00], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );
    attributes.push_back ( { CommonAttributeNames[ geomData::TCoord01], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );

    Dim xEndInd = xEnd - xStart;
    Dim yEndInd = yEnd - yStart;
    
    geomData::SizeType numVals = ( xEnd - xStart ) * ( yEnd - yStart );
    geomData::SizeType numInds = ( xEndInd - xStart ) * ( yEndInd - yStart ) * 6;

    geomData::SizeType tmpValueStride = mGeomData->getAttributes ().getValueStride ();
    mGeomData->resize ( numVals * tmpValueStride, numInds );
    
      // Now fill in indices.  These only change when the image size
      // changes, and we're not going to allow that for now.
    geomData::Indices& ind = mGeomData->getIndices ();
    geomData::SizeType* pInd = &ind[ 0 ];

    for ( Dim cury = xStart; cury < yEndInd; ++cury )
    {
      for ( Dim curx = yStart; curx < xEndInd; ++curx )
      {
        geomData::SizeType val = 
            ( cury - yStart ) * valuePitch + ( curx - xStart );
        *pInd++ = val;
        *pInd++ = val + 1;
        *pInd++ = val + valuePitch;
        
        *pInd++ = val + valuePitch;
        *pInd++ = val + 1;
        *pInd++ = val + valuePitch + 1;
      }
    }
  }

  geomData::SizeType valueStride = mGeomData->getAttributes ().getValueStride ();

PNIPSTDLOG

  assert ( sizeof ( tVert ) == valueStride * 4 );
  
  cellArgs args;
  args.mUvDx = 1.0f / width;
  args.mUvDy = 1.0f / height;
  args.mDz = mDz;
    
    // Fill in vert values.
  img::base::buffer* pBuf = mHeightImg->mBuffers[ 0 ].get ();
  for ( Dim cury = yStart; cury < yEnd; ++cury )
  {
    size_t vertStart = ( cury - yBase ) * valuePitch + ( xStart - xBase );
    args.mVert = reinterpret_cast< tVert* > ( 
        &mGeomData->getValues ()[ vertStart * valueStride ] );

    args.mXpos = xStart;
    args.mYpos = cury;

    args.x0y0 = & ( *pBuf )[ args.mYpos * pitch + args.mXpos ];
    args.xpy0 = args.x0y0 + 1;
    args.x0yp = args.x0y0 + pitch;
    args.xny0 = args.x0y0 - 1;
    args.x0yn = args.x0y0 - pitch;
    
    for ( Dim curx = xStart; curx < xEnd; ++curx )
    {
      addVert ( args );

      args.incrX ();
    }
  }

  this->setGeomBoundsDirty ();
PNIPSTDLOG
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//  overrides from scene::geom

void hterrain::generateGeomBounds () const
{
  mBounds.setEmpty ();
  
  if ( mHeightImg )
  {
    pni::math::vec3 minVal ( 1.0f, 1.0f, 0.0f );
    pni::math::vec3 maxVal ( 
        mHeightSize[ 0 ] - 2.0f, 
        mHeightSize[ 1 ] - 2.0f, 
        mDz * 0xff );
        
    mBounds.set ( minVal, maxVal );
    
    //if ( ! getTravData ( Isect ) )
    //{
    //  hterrain* pTerr = const_cast< hterrain* > ( this );
    //  pTerr->genIsectSimplePartition (); 
    //}
  }
}

/////////////////////////////////////////////////////////////////////

  // To be used for genIsectSimplePartition (at least) to figure
  // out starting index of verts in tris to be binned in collision
  // optimization partitions.
  // GOTCHA: Currently no bounds checking!!!
geomData::SizeType hterrain::xyToGeomValueIndex ( Dim xVal, Dim yVal )
{
//  size_t const DataStride = mGeomData->getValueStride ();
  Dim const xStart = 1;
  Dim const yStart = 1;
  Dim const xEnd = mHeightSize[ 0 ] - 1;
//  Dim const yEnd = mHeightSize[ 1 ] - 1;
  Dim const xStride = xEnd - xStart;
//  size_t const DataPitch = xStride * DataStride;
  size_t const TrisPerCell = 6;
  
  xVal -= xStart;
  yVal -= yStart;
  
  //geomData::SizeType ret = 
      //( geomData::SizeType ) ( yVal * DataPitch + xVal * DataStride );
  
  geomData::SizeType ret = 
      ( geomData::SizeType ) ( ( yVal * xStride + xVal ) * TrisPerCell );
  
  return ret;
}

void hterrain::generateGeomPartition () const
{
  hterrain* thisMutable = const_cast< hterrain* > ( this );

    // Right now, we'll just do the most naive thing...
    // We'll walk the tris and add them in order until
    // the current bin is filled.  So, this won't be a nice
    // even distribution as if the terrain was divided in
    // 4x4... but it will be very effective and fast to
    // execute.  Even though it's not even, the distribution
    // of tris in hterrain is extremely coherent.

  isectSimplePartition* pParts = new isectSimplePartition;
  
    // We actually throw away the
    // edge pixels from all sides of the height map.
  size_t const TotalTris = 
        ( mHeightSize[ 0 ] - 2 ) 
      * ( mHeightSize[ 1 ] - 2 ) * 2;
  size_t const NumBins = 
      ( size_t ) ( pni::math::Trait::sqrt ( ( float ) TotalTris ) * 6.0f );
  size_t const TrisPerBin = TotalTris / NumBins;
  size_t const VertsPerBin = TrisPerBin * 3;
  
  geomData::SizeType ind = 0;
  size_t count = 0;
  
  pParts->mPartitions.resize ( 1 );
  isectSimplePartition::Partition* pPart = &pParts->mPartitions.back ();
  pPart->mIndices.reserve ( VertsPerBin );
  
  triIter iter ( thisMutable );
  
  while ( iter )
  {
    pni::math::vec3 pos ( &iter );
    pPart->mBounds.extendBy ( pos );
    pPart->mIndices.push_back ( ind++ );
    ++iter;
    
    pos.set ( &iter );
    pPart->mBounds.extendBy ( pos );
    pPart->mIndices.push_back ( ind++ );
    ++iter;
    
    pos.set ( &iter );
    pPart->mBounds.extendBy ( pos );
    pPart->mIndices.push_back ( ind++ );
    ++iter;
    
    count += 1;
    
    if ( count == TrisPerBin )
    {
      count = 0;
      
      pParts->mPartitions.resize ( pParts->mPartitions.size () + 1 );
      pPart = &pParts->mPartitions.back ();
      pPart->mIndices.reserve ( VertsPerBin );
    }
  }
  
    // Fixup bounds min/max[ 2 ] to match the min and max height
    // of the height terrain field.  Since the partitions are static,
    // it has to represent the full possible extent in the vertical
    // direction to not cause problems.
  typedef isectSimplePartition::Partitions::iterator PartIter;
  PartIter end = pParts->mPartitions.end ();
  for ( PartIter cur = pParts->mPartitions.begin (); cur != end; ++cur )
  {
    pni::math::vec3 minVal ( pni::math::vec3::NoInit );
    pni::math::vec3 maxVal ( pni::math::vec3::NoInit );
    cur->mBounds.get ( minVal, maxVal );
    minVal[ 2 ] = 0.0f;
    //maxVal[ 2 ] = mDz * 0xff;   // HACK: Terrain never grows in this LD50!
    cur->mBounds.set ( minVal, maxVal );
  }

    // Figure out cube root of number of partitions... that's how
    // many items we want in each partition optimization when we're
    // doing two optimize passes.  
  float fcount = pni::math::Trait::pow ( ( float ) pParts->mPartitions.size (), 1.0f / 3.0f );
  size_t icount = ( size_t ) fcount;

  pParts->optimize ( icount );
  pParts->optimize ( icount );
  
  thisMutable->setTravData ( Isect, pParts );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

    //ImgRef mDiffImg;
    //ImgRef mFxImg;
    //ImgRef mHeightImg;
    //
    //TexRef mDiffTex;
    //TexRef mFxTex;

void hterrain::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  scene::geom::collectRefs ( refs );
  
  refs.push_back ( mDiffImg.get () );
  refs.push_back ( mFxImg.get () );
  refs.push_back ( mHeightImg.get () );
  refs.push_back ( mDiffTex.get () );
  refs.push_back ( mFxTex.get () );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


