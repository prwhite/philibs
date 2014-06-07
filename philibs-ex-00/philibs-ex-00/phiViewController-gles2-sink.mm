//
//  phiViewController.m
//  philibs-ex-00
//
//  Created by Payton White on 12/13/13.
//  Copyright (c) 2013 PREHITI HEAVY INDUSTRIES. All rights reserved.
//

#import "phiViewController.h"

#include "philibs/sceneloaderase.h"
#include "philibs/scenecamera.h"
#include "philibs/scenegroup.h"
#include "philibs/pnisphere.h"
#include "philibs/scenedepth.h"
#include "philibs/scenelighting.h"
#include "philibs/scenelight.h"
#include "philibs/scenecull.h"
#include "philibs/scenelightpath.h"
#include "philibs/scenetexture.h"
#include "philibs/scenecommon.h"
#include "philibs/scenelines.h"

#include "philibs/sceneprog.h"
#include "philibs/sceneuniform.h"

#include "philibs/pnimathstream.h"
#include "philibs/scenedbgdd.h"

#include "philibs/imgfactory.h"
#include "philibs/sceneloaderfactory.h"
#include "philibs/sceneprogfactory.h"

#include "philibs/pniosxplatform.h"

#include "philibs/sceneddogl.h"
#include "philibs/sceneframebufferogl.h"
#include "philibs/scenerendersink.h"
#include "philibs/scenerendersinkdd.h"

#include "philibs/fontfontfactory.h"
#include "philibs/scenetext.h"
#include "philibs/scenetextfactory.h"

#include "philibs/pnidbg.h"
#include <iostream>
#include <fstream>

@interface phiViewController () {
  
  pni::pstd::autoRef< scene::node > mMainGeom;
  pni::pstd::autoRef< scene::node > mRoot;
  pni::pstd::autoRef< scene::node > mFile;
  pni::pstd::autoRef< scene::prog > mProg;
  pni::pstd::autoRef< scene::uniform > mUniform00;
  pni::pstd::autoRef< scene::camera > mCam;
  pni::pstd::autoRef< scene::lines > mLines;

  pni::pstd::autoRef< scene::renderSink > mMainSink;
  pni::pstd::autoRef< scene::renderSink > mTexSink;

//  scene::framebufferOgl* mFb;
  loader::factory::LoadFuture mLoadFuture;
  
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation phiViewController

- (void)viewDidLoad
{
  [super viewDidLoad];
  
  self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  
  if (!self.context) {
    NSLog(@"Failed to create ES context");
  }
  
  self.preferredFramesPerSecond = 60;
  
  GLKView *view = (GLKView *)self.view;
  view.context = self.context;
  view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
  
  [self setupGL];
}

- (void)dealloc
{
  [self tearDownGL];
  
  if ([EAGLContext currentContext] == self.context) {
    [EAGLContext setCurrentContext:nil];
  }
}

- (void)didReceiveMemoryWarning
{
  [super didReceiveMemoryWarning];
  
  if ([self isViewLoaded] && ([[self view] window] == nil)) {
    self.view = nil;
    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
      [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
  }
  
  // Dispose of any resources that can be recreated.
}

#pragma mark - Setup scene rendering and scene graph


scene::texture* loadCubemap ()
{
  scene::texture* pTex = new scene::texture;
  pTex->setTarget( scene::texture::CubeMapTarget );
  pTex->setName("PalmTrees Cube Map");
  pTex->setMinFilter( scene::texture::MinLinearMipNearest );

    // The order here matches the order in the ImageId enum so that
    // the cubeSide counter variable will be in the matching sequence.
  std::vector< std::string > imgs {
    "PalmTrees/posx.jpg",
    "PalmTrees/negx.jpg",
    "PalmTrees/posy.jpg",
    "PalmTrees/negy.jpg",
    "PalmTrees/posz.jpg",
    "PalmTrees/negz.jpg",
  };
  
  uint32_t cubeSide = 0;
  for ( auto iter : imgs )
  {
    pTex->setImage(
        img::factory::getInstance().loadSync(iter),
        static_cast< scene::texture::ImageId > ( cubeSide++ ));
  }
  
  return pTex;
}

scene::geom* buildQuad ()
{
  using namespace scene;

  geom* pMainGeom = new geom;
  pMainGeom->setName("final scene quad");

  geomData* pGeomData = new geomData;
  pGeomData->attributesOp().push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );
//  pGeomData->attributesOp().push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );
  pGeomData->attributesOp().push_back ( { CommonAttributeNames[ geomData::TCoord00], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );
//  pGeomData->attributesOp().push_back ( { CommonAttributeNames[ geomData::TCoord01], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );

  pGeomData->resizeTrisWithCurrentAttributes(4, 2);
  
  float QuadSize = 1.0f;
  
    /// x,y,z,u,v
  pGeomData->getValues() = {
    -QuadSize, -QuadSize, 0.0f,   0.0f, 0.0f,
     QuadSize, -QuadSize, 0.0f,   1.0f, 0.0f,
    -QuadSize,  QuadSize, 0.0f,   0.0f, 1.0f,
     QuadSize,  QuadSize, 0.0f,   1.0f, 1.0f
  };
  
  pGeomData->getIndices() = {
    0, 1, 2, 1, 3, 2
  };
  
  pMainGeom->setGeomData(pGeomData);
  
  return pMainGeom;
}

scene::prog* createMainProg ()
{
  using namespace scene;
  
  prog* pProg = progFactory::getInstance().loadSync({
    "Shaders/gles2-sink-postproc.vsh",
    "Shaders/gles2-sink-postproc.fsh"
  });
  
  return pProg;
}

void buildLines ( scene::node* pRoot )
{
  using namespace scene;
  using namespace pni::math;
  
  float const Zoff = 1.25f;
  float const Thick = 10.0f;
  
  lineData* pLineData = new lineData;
  
  pLineData->mBinding.push_back ( { {}, lineData::Position, lineData::Float, sizeof(float), 3 } );
  pLineData->mBinding.push_back ( { {}, lineData::Color, lineData::Float, sizeof(float), 4 } );
  pLineData->mBinding.push_back ( { {}, lineData::Thickness, lineData::Float, sizeof(float), 1 } );
  
    // Create a half circle
  for ( float degree = 0.0f; degree < 180.0f; degree += 5.0f )
  {
    vec3 pt { 1.0f, 0.0f, Zoff };
    pni::math::matrix4 mat;
    
    mat.setRot(degree, 0.0f, 0.0f, 1.0f);
    pt.xformPt(pt, mat);
    pt[ 0 ] *= 0.1f;
    
    size_t oldSize = pLineData->size();
    size_t newSize = oldSize + 1;
    pLineData->resize(newSize, 1);
    pLineData->getIndices()[ 0 ] = newSize;
    
    auto pos = pLineData->begin< pni::math::vec3 >(lineData::Position);
    auto col = pLineData->begin< pni::math::vec4 >(lineData::Color);
    auto thk = pLineData->begin< float > (lineData::Thickness);

    pos += oldSize; col += oldSize; thk += oldSize;

    *pos = pt;
    col->set ( 0.0f, 0.0f, 0.0f, 1.0f );
    *thk = Thick;
  }
  
    // Put in end point below half circle
  {
    size_t oldSize = pLineData->size();
    size_t newSize = oldSize + 1;
    pLineData->resize(newSize, 1);
    pLineData->getIndices()[ 0 ] = newSize;

    auto pos = pLineData->begin< pni::math::vec3 >(lineData::Position);
    auto col = pLineData->begin< pni::math::vec4 >(lineData::Color);
    auto thk = pLineData->begin< float > (lineData::Thickness);

    pos += oldSize; col += oldSize; thk += oldSize;

    pos->set ( 0.0f, -1.0f, Zoff );
    col->set ( 0.0f, 0.0f, 0.0f, 1.0f );
    *thk = Thick;
  }
  
  lineData* pLineDataOrig = pLineData;
  
    // Default style
  lines* pLines = new lines;
  pLines->lineDataProp() = pLineData;
  pLines->setViewportSizeRatio( { 1.0, 1024.0 / 1536.0 } );
  prog* pLineProg = progFactory::getInstance().loadSync( { "gles2-line.vsh", "gles2-line.fsh" } );
  pLines->setState(pLineProg, scene::state::Prog);
  pLines->setState(new scene::blend, state::Blend);
  pLines->matrixOp().setTrans(-2.0f, 0.0f, 0.0f);
  pLines->lineStyleProp().op().mEdgeRange = 0.3f;
  pLines->setName("aa");
  pRoot->addChild ( pLines );
  
    // Thinner line
  pLines = pLines->dup();
  pLines->uniformProp() = pLines->uniformProp()->dup();
  pLines->matrixOp().setTrans(-1.5f, 0.0f, 0.0f);
  pLines->setName("bb");
  pLines->lineStyleProp().op().mEdgeMiddle = 0.5f;
  pLines->lineStyleProp().op().mEdgeRange = 0.25f;
  pRoot->addChild ( pLines );

    // Very thin line
  pLines = pLines->dup();
  pLines->uniformProp() = pLines->uniformProp()->dup();
  pLines->matrixOp().setTrans(-1.0f, 0.0f, 0.0f);
  pLines->setName("cc");
  pLines->lineStyleProp().op().mEdgeMiddle = 0.8f;
  pLines->lineStyleProp().op().mEdgeRange = 0.2f;
  pRoot->addChild ( pLines );

    // Default thickness dashed line
  pLines = pLines->dup();
  pLines->uniformProp() = pLines->uniformProp()->dup();
  pLines->matrixOp().setTrans(2.0f, 0.0f, 0.0f);
  pLines->setName("cc");
  pLines->lineStyleProp().op().mEdgeMiddle = 0.2f;
  pLines->lineStyleProp().op().mEdgeRange = 0.3f;
  pLines->lineStyleProp().op().mEnableFlags = lineStyle::Dash00;
  pRoot->addChild ( pLines );
  
    // Very thin line
  pLines = pLines->dup();
  pLines->uniformProp() = pLines->uniformProp()->dup();
  pLines->matrixOp().setTrans(1.0f, 0.0f, 0.0f);
  pLines->setName("ff");
  pLines->lineStyleProp().op().mEdgeMiddle = 0.8f;
  pLines->lineStyleProp().op().mEdgeRange = 0.2f;
  pRoot->addChild ( pLines );
  
    // Thinner line, shorter dashes
  pLines = pLines->dup();
  pLines->uniformProp() = pLines->uniformProp()->dup();
  pLines->matrixOp().setTrans(0.5f, 0.0f, 0.0f);
  pLines->setName("gg");
  pLines->lineStyleProp().op().mEdgeMiddle = 0.25f;
  pLines->lineStyleProp().op().mEdgeRange = 0.1f;
  pLines->lineStyleProp().op().mDashMiddle = 0.5f;
  pLines->lineStyleProp().op().mDashRange = 0.1f;
  pLines->lineStyleProp().op().mDashPeriod = 0.025f;
  pRoot->addChild ( pLines );

    // Very thin line, round-ish soft dashes
  pLines = pLines->dup();
  pLines->uniformProp() = pLines->uniformProp()->dup();
  pLines->matrixOp().setTrans(0.0f, 0.0f, 0.0f);
  pLines->setName("hh");
  pLines->lineStyleProp().op().mEdgeMiddle = 0.4f;
  pLines->lineStyleProp().op().mEdgeRange = 0.4f;
  pLines->lineStyleProp().op().mDashMiddle = 0.5f;
  pLines->lineStyleProp().op().mDashRange = 0.4f;
  pLines->lineStyleProp().op().mDashPeriod = 0.0125f;
  pRoot->addChild ( pLines );

    // Wavy, multi-colored
  pLines = pLines->dup();
  pLines->uniformProp() = pLines->uniformProp()->dup();
  pLines->matrixOp().setTrans(-0.5f, 0.0f, 0.0f);
  pLines->setName("ii");
  pLines->lineStyleProp().op() = {};
  pLines->lineStyleProp().op().mEnableFlags = lineStyle::Dash00;
  pLines->lineStyleProp().op().mDashMiddle = 0.5f;
  pLines->lineStyleProp().op().mDashRange = 0.1f;
  pLines->lineStyleProp().op().mDashPeriod = 0.025f;
  pLines->lineDataProp().set ( new lineData ( *pLines->lineDataProp().get() ) );
  pRoot->addChild ( pLines );

    // Change the line metadata
  pLineData = pLines->lineDataProp().op();

  pni::math::vec4 const colors [] = { { 1, 0, 0, 1 }, { 1, 1, 0, 1 }, { 0, 1, 0, 1 }, { 0, 1, 1, 1 }, { 0, 0, 1, 1 }, { 1, 0, 1, 1 } };
  size_t const numColors = sizeof ( colors ) / sizeof ( decltype(colors[0]));
  static_assert ( numColors == 6, "unexpected number of colors calculated" );
  float thicknii [] = { 1.0f, 1.25, 1.4, 1.3, 1.1, 0.8, 0.75, 1.0, 1.5, 2.0, 1.25, 0.7f };
  size_t const numThicknii = sizeof ( thicknii ) / sizeof ( decltype(thicknii[0]));
  static_assert ( numThicknii == 12, "unexpected number of thicknii calculated" );

  size_t end = pLineData->size();
  for ( int num = 0; num < end; ++num )
  {
    auto citer = pLineData->begin< pni::math::vec4 >( lineData::Color );
    auto titer = pLineData->begin< float >( lineData::Thickness );
  
    citer += num; titer += num;
    
    *citer = colors[ num % numColors ];
    *titer = thicknii[ num % numThicknii] * Thick;
  }

    // Textured line
  pLines = pLines->dup();
  pLines->uniformProp() = pLines->uniformProp()->dup();
  pLines->matrixOp().setTrans(1.5f, 0.0f, 0.0f);
  pLines->setName("jj");
  pLines->lineStyleProp().op() = {};
  pLines->lineStyleProp().op().mEnableFlags = lineStyle::Tex00;
  pLines->lineStyleProp().op().mTexUMult = 2.0f;
  pLines->lineStyleProp().op().mTexMiddle = 0.8f;
  pLines->lineStyleProp().op().mTexRange = 0.2f;
  pLines->lineDataProp().set ( new lineData ( *pLineDataOrig ) );
  
    // Make the line thicker to show of the texture
  auto tbeg = pLines->lineDataProp().get()->begin< float >(lineData::Thickness );
  auto const tend = pLines->lineDataProp().get()->end< float >(lineData::Thickness );
  while ( tbeg != tend )
  {
    *tbeg *= 3.0f;
    ++tbeg;
  }

    // Uniquify the program object because doing texture changes the prog text
  prog* pProg = static_cast< prog* > ( pLines->getState( state::Prog ) );
  pLines->setState(pProg->dup(), state::Prog);

  texture* pTex = new texture;
  pTex->setImage(img::factory::getInstance().loadSync("line-tex-6a3-00a-gray.png"));
  pTex->setMinFilter( texture::MinLinearMipNearest );
  pLines->setState(pTex, state::Texture00);

  pRoot->addChild ( pLines );
}

- (void) setupScene
{
  using namespace scene;

    // Path to the app bundle to get the test file.
  std::string bdir ( getShellPath(BundleDir) );
  
    // Add the app bundle to all of the factories' search paths
  loader::factory::getInstance().mSearchPath += bdir;
  img::factory::getInstance().mSearchPath =
      progFactory::getInstance().mSearchPath =
      font::fontFactory::getInstance().mSearchPath =
      loader::factory::getInstance().mSearchPath;

  GLKView *view = (GLKView *)self.view;

    // h & w swapped for landscape
  img::base::Dim width  = (img::base::Dim)view.drawableHeight;
  img::base::Dim height = (img::base::Dim)view.drawableWidth;

    // For size of texture framebuffer... we are doing a mip-map trick, so we want PoT
  img::base::Dim width2 = 2048;
  img::base::Dim height2 = 1024;

    ////////////////////////////////////////////////
    // set up main scene... will just be a quad with a rt texture
  img::base* pMainSceneImg = new img::base;
//  pMainSceneImg->setSize(width, height, width);
  pMainSceneImg->setSize(width2, height2, width2);
  pMainSceneImg->setFormat(img::base::RGB565);

  texture* pMainSceneTex = new texture;
  pMainSceneTex->setName("tex scene dest");
  pMainSceneTex->setImage(pMainSceneImg);
  pMainSceneTex->setWrap(texture::ClampToEdge, texture::ClampToEdge, texture::ClampToEdge);
  pMainSceneTex->setMinFilter(texture::MinLinearMipLinear);

  node* pMainGeom = buildQuad();
  pMainGeom->setState(pMainSceneTex, state::Texture00);
  mMainGeom = pMainGeom;      // store this so we can do stuff with it in the update loop.

  camera* pMainCam = new camera;
  pMainCam->setOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
  pMainCam->setViewport(0.0f, 0.0f, width, height);
  pMainCam->setColorClear( { 0.2f, 0.0f, 0.0f, 1.0f } );
  
  prog* pMainProg = createMainProg();
  
  node* pMainRoot = new group;

  pMainRoot->setState(pMainProg, state::Prog);
  pMainRoot->setState(new cull, state::Cull);
  
  pMainRoot->addChild(pMainGeom);
  pMainRoot->addChild(pMainCam);

  framebufferOgl* pFb = new framebufferOgl;
  pFb->captureDefaultFb();
  pFb->specOp().mWidth = width;
  pFb->specOp().mHeight = height;

  self->mMainSink = new renderSink;
  self->mMainSink->mDrawSpec.mDd = new ddOgl;
  self->mMainSink->mDrawSpec.mRootNode = pMainRoot;
  self->mMainSink->mDrawSpec.mSinkPath = pMainCam;
  self->mMainSink->mFramebuffer = pFb;
  self->mMainSink->mFramebuffer->setName("main");

    //////////////////////////////////////////////////
    // Now the "real" scene with the loaded file, etc.

    // Load the file, grab its bounding sphere so we can push back the camera an
    // appropriate amount.
  mLoadFuture = loader::factory::getInstance().loadAsync("test-00b.ase");
//  mLoadFuture = loader::factory::getInstance().loadAsync("cyclorama-00a.dae");
//  mLoadFuture = loader::factory::getInstance().loadAsync("ld-pod-01b-hi-icon.dae");


  mRoot = new scene::group ();

    // Add default vert and frag programs
  mProg = new scene::prog;
  
  mProg->setFlag(scene::prog::Tex, 00);
  mProg->setFlag(scene::prog::Uv, 00);
  mProg->setFlag(scene::prog::Sampler2D, 00);

  mProg->setFlag(scene::prog::Tex, 01);
  mProg->setFlag(scene::prog::UvReflection, 01);
  mProg->setFlag(scene::prog::SamplerCube, 01);

  mProg->setFlag(scene::prog::Normal);
  
  mProg->setDefaultProgs();
  mRoot->setState(mProg.get(), scene::state::Prog);

    // Create a default depth state object.
  scene::depth* pDepth = new scene::depth ();
  mRoot->setState(pDepth, scene::state::Depth);
  
  scene::texture* pCubemap = loadCubemap ();
  pCubemap->setOverride(true);
  mRoot->setState(pCubemap, scene::state::Texture01);
  
    // Create and setup the camera.
  mCam = new scene::camera ();
  mCam->setColorClear( { 0.2f, 0.2f, 0.3f, 1.0f } );
  mCam->setNormalizeMode( scene::camera::Normalize );
  mCam->setViewport( 0.0f, 0.0f, width2, height2 );

  scene::cull* pCull = new scene::cull;
  mRoot->setState(pCull, scene::state::Cull);

  mRoot->addChild ( mCam.get () );

    // setup render to texture scene
  self->mTexSink = new renderSink;
  self->mTexSink->mDrawSpec.mDd = new ddOgl;
  self->mTexSink->mDrawSpec.mSinkPath = mCam.get();
  self->mTexSink->mDrawSpec.mRootNode = mRoot.get();

  self->mTexSink->mFramebuffer = new framebufferOgl;
  framebuffer::spec& tspec = self->mTexSink->mFramebuffer->specOp();
  tspec.mTextureTarget                  = texture::Tex2DTarget;
  tspec.mColorType[ 0 ]                 = framebuffer::Texture;
  tspec.mColorAttachment[ 0 ]           = framebuffer::ColorAttachmentRGB565;
  tspec.mDepthType                      = framebuffer::Renderbuffer;
  tspec.mDepthAttachment                = framebuffer::DepthAttachment16;
  tspec.mDestinationBuffer              = framebuffer::Front;
  tspec.mWidth                          = width2;
  tspec.mHeight                         = height2;
  self->mTexSink->mFramebuffer->setColorTextureTarget(0, pMainSceneTex);
  self->mTexSink->mFramebuffer->setName("render to texture scene");

  self->mMainSink->addChild(self->mTexSink.get());
 
//  font::font* pFont = font::fontFactory::getInstance().loadSync(
//        "HelveticaNeue.dfont_sdf.txt", "HelveticaNeue.dfont_sdf_g8.png");
 
  font::font* pFont = font::fontFactory::getInstance().loadSync(
      "Helvetica_Neue.fnt", "Helvetica_Neue.png_g8.png");
  
  text* pText = textFactory::getInstance ().newText(pFont);
  
  uniform* pUniform = ( uniform* ) pText->getState(state::Uniform00);

  commonUniformOp< float > ( pUniform, UniformTextMin ) = 0.45f;
  commonUniformOp< float > ( pUniform, UniformTextMax ) = 0.65f;
  commonUniformOp< pni::math::vec4 > ( pUniform, UniformTextColor ).set (
    0.5f, 0.5f, 1.0f, 0.25f );
  
  pText->setText("©PHI");
  
  scene::depth* pNoDepth = new scene::depth;
  pNoDepth->setEnable(false);
  pText->setState(pNoDepth, state::Depth);
  
  pText->setState(new blend, state::Blend);
  
  pText->matrixOp().setTrans(75.0f, -70.0f, -125.0f);
  
  float const scale = 16.0f;
  pText->matrixOp().preScale(scale, scale, scale);
  
  mRoot->addChild(pText);
}

- (void) finalizeScene
{
  using namespace scene;

  mFile = mLoadFuture.get();

#ifdef DBGFILE
  std::string ofile ( getShellPath(BundleDir) + "/scene-dbg.txt" );
  std::ofstream ostr ( ofile );
  scene::dbgDd dbg ( ostr );
  dbg.setDbgOpts(scene::dbgDd::All);
  dbg.startGraph(mFile.get());
  std::cout << ofile << std::endl;
#endif // DBGFILE

  std::cout << "mFile bounds is " << mFile->getBounds() << std::endl;
  pni::math::sphere boundingSphere;
  boundingSphere.extendBy(mFile->getBounds());

    // Add a uniform that will do something debuggy
  mUniform00 = new scene::uniform;
    // ...add some bindings for debugging...
  mRoot->setState(mUniform00.get(), scene::state::Uniform00);

    // Enable basic lighting state.
  scene::lighting* pLighting = new scene::lighting;
  pLighting->setAmbient ( pni::math::vec4 ( 0.2f, 0.2f, 0.2f, 1.0f ) );  // TEMP
  mRoot->setState ( pLighting, scene::state::Lighting );
  
    // Update cam position
  mCam->matrixOp().setTrans ( 0.0f, 0.0f, 2.0f * boundingSphere.getRadius() );

    // Create a dynamic light.
  scene::light* pLight = new scene::light;
  pLight->setDirection( pni::math::vec3 ( 1.0f, -1.0f, -1.0f ) ); // Not normalized :(
  
    // Add file and light as children to the root.
  mRoot->addChild ( mFile.get () );
  mRoot->addChild ( pLight );
  
    // Tell the root of the scene what lights affect it.
  scene::lightPath* pLightPath = new scene::lightPath ();
  pLightPath->setLight ( pLight, 0 );
  mRoot->setState ( pLightPath, scene::state::LightPath );

    ////////////////////////////////////////////////////////////////////////////
    // Lines
  lines* pLines = new lines;
  mLines = pLines;
  
  mLines->lineStyleProp().op().mEnableFlags = lines::style::Dash00;
  
  lineData* pLineData = new lineData;
  pLines->lineDataProp().set ( pLineData );
  
  pLineData->mBinding.push_back ( { {}, lineData::Position, lineData::Float, sizeof(float), 3 } );
  pLineData->mBinding.push_back ( { {}, lineData::Color, lineData::Float, sizeof(float), 4 } );
  pLineData->mBinding.push_back ( { {}, lineData::Thickness, lineData::Float, sizeof(float), 1 } );

  pLineData->resize(0,0);

  pLines->setViewportSizeRatio( { 1.0, 1024.0 / 1536.0 } );

  prog* pLineProg = progFactory::getInstance().loadSync( { "gles2-line.vsh", "gles2-line.fsh" } );
  pLines->setState(pLineProg, scene::state::Prog);
  
  pLines->setState ( new blend, scene::state::Blend );
  
  depth* pDepth = new depth;
  pDepth->setDepthFunc ( scene::depth::LEqual );
  pLines->setState ( pDepth, scene::state::Depth );
  
  pLines->setName("line test");
  
  buildLines(mRoot.get());
  
  mFile->addChild(pLines);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#pragma mark - Setup and teardown GL resources

- (void)setupGL
{
  [EAGLContext setCurrentContext:self.context];

    // Bind the GLKView's framebuffer so we can capture it for later restore.
  GLKView* glView = ( GLKView* ) self.view;
  [glView bindDrawable];

  glView.contentScaleFactor = 2.0;

  GLubyte const* glesVer = glGetString(GL_VERSION);
  PNIDBGSTR("GLES version is: " << glesVer);
  
  GLubyte const* glslVer = glGetString(GL_SHADING_LANGUAGE_VERSION);
  PNIDBGSTR("GLSL version (for GLES 2.0) is: " << glslVer);

  [self setupScene];
}

- (void)tearDownGL
{
  [EAGLContext setCurrentContext:self.context];
  
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
  if ( ! mFile )
  {
    if ( mLoadFuture.wait_for( std::chrono::seconds ( 0 ) ) == std::future_status::ready )
      [self finalizeScene];
  }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    // Very cheesy rotation animation.
  static float rot = 0.0f;
  rot += 0.2f;

  pni::math::vec3 axis { 0.25f, 0.5f, 1.0f };
  axis.normalize();

  float const Incr = 0.1f;
  if ( mLines )
    mLines->lineStyleProp().op().mDashPhase =
      mLines->lineStyleProp()->mDashPhase > pni::math::Trait::d2r( 2.0 ) ?
        mLines->lineStyleProp()->mDashPhase - pni::math::Trait::d2r( 2.0 ) + Incr :
        mLines->lineStyleProp()->mDashPhase + Incr;

  if ( mFile )
    mFile->matrixOp().setRot(rot, axis);
  
//  mMainGeom->matrixOp().setRot(rot, axis);

    // Invoke the rendering pass.
  scene::renderSinkDd* rsDd = new scene::renderSinkDd;
  rsDd->startGraph(mMainSink.get(), 0.0);
}

// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////

struct linePanData
{
  pni::math::vec2 mLastPos;
  
  pni::math::vec2 cg2pt ( UIPanGestureRecognizer* gr )
    {
      pni::math::vec2 ret;
      
      CGPoint pt = [gr locationInView:gr.view];
      
      ret.set ( pt.x, pt.y );
      
      pni::math::vec2 vpSize ( gr.view.frame.size.width, gr.view.frame.size.height);

      ret[ 0 ] = vpSize[ 0 ] - ret[ 0 ]; // flip y

      ret /= vpSize;
      
      ret = ret * 4.0f - 2.0f;  // Scale it up around the origin.
      
      return ret;
    }
};

- (IBAction)onPan:(id)sender
{
  using namespace scene;

  float const ThickStart = 35.0f;
  float const ThickEnd = ThickStart / 10.0f;
  float const DistMin = ThickStart / 1024.0f;
  float const ZOff = 1.125f;
  float const ThickMax = DistMin * 5.0f;

  static linePanData lpd;

  UIPanGestureRecognizer* pr = ( UIPanGestureRecognizer* ) sender;

  lineData* pLineData = mLines->lineDataProp().get();

  switch ( pr.state )
  {
    case UIGestureRecognizerStateBegan:
      {
        lpd.mLastPos = lpd.cg2pt ( pr );
        
//        mLines->mLineData.setDirty();
        pLineData->resize ( 1, 1 );

        auto pos = pLineData->begin< pni::math::vec3 >(lineData::Position);
        auto col = pLineData->begin< pni::math::vec4 >(lineData::Color);
        auto thk = pLineData->begin< float > (lineData::Thickness);
        
        pos->set ( lpd.mLastPos[ 0 ], lpd.mLastPos[ 1 ], ZOff );
        col->set ( 0.0f, 0.0f, 0.0f, 1.0f );
        *thk = ThickStart;
      }
    break;
    case UIGestureRecognizerStateChanged:
      {
        pni::math::vec2 nextPos = lpd.cg2pt ( pr );
        
        float dist = nextPos.dist(lpd.mLastPos);
        
        if ( dist > DistMin )
        {
          lpd.mLastPos = nextPos;
          
          mLines->lineDataProp().setDirty();
          
          size_t last = pLineData->getIndices()[ 0 ];
          pLineData->getIndices()[ 0 ]++;
          pLineData->resize ( pLineData->getIndices()[ 0 ], 1 );

          auto pos = pLineData->begin< pni::math::vec3 >(lineData::Position);
          auto col = pLineData->begin< pni::math::vec4 >(lineData::Color);
          auto thk = pLineData->begin< float > (lineData::Thickness);

          pos += last; col += last; thk += last;

          pos->set ( lpd.mLastPos[ 0 ], lpd.mLastPos[ 1 ], ZOff );
          col->set ( 0.0f, 0.0f, 0.0f, 1.0f );
          
          dist -= DistMin;
          float thkOut = ThickStart * ( ThickMax - dist ) / ThickMax;
          
          if ( thkOut < ThickEnd ) thkOut = ThickEnd;
          
          *thk = thkOut;
        }
      }
      break;
    default:
      {
        lpd.mLastPos = lpd.cg2pt ( pr );
        
        mLines->lineDataProp().setDirty();

        size_t last = pLineData->getIndices()[ 0 ];
        pLineData->getIndices()[ 0 ]++;
        pLineData->resize ( pLineData->getIndices()[ 0 ], 1 );

        auto pos = pLineData->begin< pni::math::vec3 >(lineData::Position);
        auto col = pLineData->begin< pni::math::vec4 >(lineData::Color);
        auto thk = pLineData->begin< float > (lineData::Thickness);

        pos += last; col += last; thk += last;

        pos->set ( lpd.mLastPos[ 0 ], lpd.mLastPos[ 1 ], ZOff );
        col->set ( 0.0f, 0.0f, 0.0f, 1.0f );
        
        *thk = ThickEnd;
      }
      break;
  }
}

@end
