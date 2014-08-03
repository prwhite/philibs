/////////////////////////////////////////////////////////////////////
//
//    file: scenerendersink.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenerendersink.h"
#include "pnirefcountdbg.h"
#include "scenerendersinkdd.h"
#include "scenefactory.h"
#include "scenegroup.h"
#include "sceneprogfactory.h"
#include "scenedepth.h"
#include "scenecull.h"
#include "scenecamera.h"
#include "scenecommon.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

//renderSink::renderSink()
//{
//    
//}
//
//renderSink::~renderSink()
//{
//    
//}
//
//renderSink::renderSink(renderSink const& rhs)
//{
//    
//}
//
//renderSink& renderSink::operator=(renderSink const& rhs)
//{
//    
//    return *this;
//}
//
//bool renderSink::operator==(renderSink const& rhs) const
//{
//    
//    return false;
//}

void renderSink::accept ( renderSinkDd* pDd ) const
{
  pDd->dispatch ( this );
}


/////////////////////////////////////////////////////////////////////
// overrides from pni::pstd::travDataContainer

void renderSink::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
    // TODO: This is way incomplete!
  pni::pstd::dbgRefCount::collectMapSecondRefs ( getTravDatum (), refs );
  mTextureTargets.collectRefs(refs);
  mTextureSources.collectRefs(refs);
}



// ///////////////////////////////////////////////////////////////////

namespace { // anonymous

  // Build a quad that is always uv mapped [0,1] and pos mapped [-1,1].
scene::geom* buildQuad ()
{
  using namespace scene;

  geom* pMainGeom = new geom;

  geomData* pGeomData = new geomData;
  pGeomData->mBinding.push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, sizeof(float), geomData::PositionComponents } );
  pGeomData->mBinding.push_back ( { CommonAttributeNames[ geomData::TCoord], geomData::TCoord, geomData::DataType_FLOAT, sizeof(float), geomData::TCoordComponents, 0 } );

  pGeomData->resizeTrisWithCurrentBinding(4, 2);
  
  float QuadSize = 1.0f;
  
  struct qVert { pni::math::vec3 pos; pni::math::vec2 uv; };
  
  vertIter viter { pGeomData };
  
  viter.get<qVert>(0) = { { -QuadSize, -QuadSize, 0.0f }, {   0.0f, 0.0f } }; ++viter;
  viter.get<qVert>(0) = { {  QuadSize, -QuadSize, 0.0f }, {   1.0f, 0.0f } }; ++viter;
  viter.get<qVert>(0) = { { -QuadSize,  QuadSize, 0.0f }, {   0.0f, 1.0f } }; ++viter;
  viter.get<qVert>(0) = { {  QuadSize,  QuadSize, 0.0f }, {   1.0f, 1.0f } }; ++viter;
  
  pGeomData->getIndices() = {
    0, 1, 2, 1, 3, 2
  };
  
  pMainGeom->setGeomData(pGeomData);
  
  return pMainGeom;
}


}

// ///////////////////////////////////////////////////////////////////

void renderSink::initScene ( DimPair const size, TravMaskType mask )
{
  group* pRoot = new group ();

    // Add default vert and frag programs
  prog* pProg = new prog;
  pProg->setFlag(prog::Normal);
  pProg->setDefaultProgs();
  pRoot->setState(pProg, state::Prog);

    // Create a default depth state object.
  depth* pDepth = new depth ();
  pRoot->setState(pDepth, state::Depth);
  
  cull* pCull = new cull;
  pRoot->setState(pCull, state::Cull);

    // Create and setup the camera.
  camera* pCam = new camera ();
  pCam->setColorClear( { 0.2f, 0.2f, 0.2f, 1.0f } );
  pCam->setNormalizeMode( camera::Normalize );
  pCam->setViewport( 0.0f, 0.0f, size[ 0 ], size[ 1 ] );

  pRoot->addChild ( pCam );

  mDrawSpec.mDd = factory::getInstance()->newGraphDd();
  mDrawSpec.mSinkPath = pCam;
  mDrawSpec.mRootNode = pRoot;
  mDrawSpec.mTravMask = mask;
}

void renderSink::initSceneDepth ( DimPair const size, renderSink* pVisual, TravMaskType mask )
{
    // Reference the visual scene's draw root, dd, camera, but we will change the trav mask
  mDrawSpec = pVisual->mDrawSpec;
  mDrawSpec.mTravMask = mask;

  node* pRoot = mDrawSpec.mRootNode.get();

    // Add default vert and frag programs
    // TODO: Load program for z-only pass, no color writes, then try to get rid
    // of color buffer and hope that we don't get prog validation problems again
//  prog* pProg = progFactory::getInstance().loadSync( {
//    "Shaders/gles2-zpass.vsh",
//    "Shaders/gles2-zpass.fsh"
//  });
  prog* pProg = new prog;
  pProg->setDefaultProgs();
  pProg->setTravMask(mask);
  pRoot->setState(pProg, state::Prog);

    // Create a default depth state object.
  depth* pDepth = new depth ();
  pDepth->setTravMask(mask);
  pRoot->setState(pDepth, state::Depth);
  
  cull* pCull = new cull;
  pCull->setTravMask(mask);
  pRoot->setState(pCull, state::Cull);
}

void renderSink::initPostProcScene ( DimPair const srcSize, DimPair const dstSize, img::base::Format format )
{
  img::base* pMainSceneImg = new img::base;
  pMainSceneImg->setSize( srcSize[ 0 ], srcSize[ 1 ], srcSize[ 0 ] );
  pMainSceneImg->setFormat(format);

  texture* pMainSceneTex = new texture;
  pMainSceneTex->setName("tex scene dest");
  pMainSceneTex->setImage(pMainSceneImg);
  pMainSceneTex->setWrap(texture::ClampToEdge, texture::ClampToEdge, texture::ClampToEdge);
    // TEMPORARILY disable mip-map generation used for lighting bloom
//  pMainSceneTex->setMinFilter(texture::MinLinearMipLinear);

  switch ( format )
  {
    case img::base::RGB565:
      mTextureSources.setColorTextureTarget(0, pMainSceneTex);
      break;
    case img::base::DEPTH_COMPONENT16:
      mTextureSources.setDepthTextureTarget(pMainSceneTex);
      break;
    default:
      PNIDBGSTR("texture format not recognized");
      break;
  }

  node* pMainGeom = buildQuad();
  pMainGeom->setName("post-proc quad");
  pMainGeom->setState(pMainSceneTex, state::Texture00);

  prog* pMainProg = progFactory::getInstance().loadSync({
    "Shaders/gles2-sink-postproc.vsh",
    "Shaders/gles2-sink-postproc.fsh"
  });
  
  pMainGeom->setState(pMainProg, state::Prog);

  camera* pMainCam = new camera;
  pMainCam->setOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
  pMainCam->setViewport(0.0f, 0.0f, dstSize[ 0 ], dstSize[ 1 ]);
  pMainCam->setClearMask( camera::BufferNone );
  
  node* pMainRoot = new group;

  pMainRoot->setState(pMainProg, state::Prog);

  pMainRoot->setState(new cull, state::Cull);

  depth* pDepth = new depth;
  pDepth->setEnable(false);
  pMainRoot->setState(pDepth, state::Depth);
  
  pMainRoot->addChild(pMainGeom);
  pMainRoot->addChild(pMainCam);

  mDrawSpec.mDd = factory::getInstance()->newGraphDd();
  mDrawSpec.mSinkPath = pMainCam;
  mDrawSpec.mRootNode = pMainRoot;
}

// ///////////////////////////////////////////////////////////////////

void renderSink::initFinalFb(const Dim *size, std::string const& name )
{
  mFramebuffer = factory::getInstance()->newFramebuffer();
  mFramebuffer->setName(name);

  framebuffer::spec& tspec = mFramebuffer->specOp();
  tspec.mWidth                          = size[ 0 ];
  tspec.mHeight                         = size[ 1 ];
}

void renderSink::initTextureFb( DimPair const size, std::string const& name  )
{
    // setup render to texture scene
  mFramebuffer = factory::getInstance()->newFramebuffer();
  mFramebuffer->setName(name);

  framebuffer::spec& tspec = mFramebuffer->specOp();
  tspec.mTextureTarget                  = texture::Tex2DTarget;
  tspec.mColorType[ 0 ]                 = framebuffer::Texture;
  tspec.mColorAttachment[ 0 ]           = framebuffer::ColorAttachmentRGB565;
  tspec.mDepthType                      = framebuffer::Renderbuffer;
  tspec.mDepthAttachment                = framebuffer::DepthAttachment24;
  tspec.mDestinationBuffer              = framebuffer::Front;
  tspec.mWidth                          = size[ 0 ];
  tspec.mHeight                         = size[ 1 ];
}

void renderSink::initTextureFbDepth ( DimPair const size, std::string const& name  )
{
    // setup render to texture scene
  mFramebuffer = factory::getInstance()->newFramebuffer();
  mFramebuffer->setName(name);

  framebuffer::spec& tspec = mFramebuffer->specOp();
  tspec.mTextureTarget                  = texture::Tex2DTarget;
  tspec.mColorType[ 0 ]                 = framebuffer::Renderbuffer;
  tspec.mColorAttachment[ 0 ]           = framebuffer::ColorAttachmentRGB565;
  tspec.mDepthType                      = framebuffer::Texture;
  tspec.mDepthAttachment                = framebuffer::DepthAttachment16;
  tspec.mDestinationBuffer              = framebuffer::Front;
  tspec.mWidth                          = size[ 0 ];
  tspec.mHeight                         = size[ 1 ];

}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


