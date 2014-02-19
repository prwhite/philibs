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
#include "philibs/imgfactory.h"
#include "philibs/scenecommon.h"

#include "philibs/sceneprog.h"
#include "philibs/sceneuniform.h"

#include "philibs/pnimathstream.h"
#include "philibs/scenedbgdd.h"

#include "philibs/sceneloaderfactory.h"

#include "pniosxplatform.h"

#include "philibs/sceneddogl.h"
#include "philibs/sceneframebufferogl.h"
#include "philibs/scenerendersink.h"
#include "philibs/scenerendersinkdd.h"

#include <iostream>
#include <fstream>

@interface phiViewController () {
  
  pni::pstd::autoRef< scene::node > mMainGeom;
  pni::pstd::autoRef< scene::node > mRoot;
  pni::pstd::autoRef< scene::node > mFile;
  pni::pstd::autoRef< scene::prog > mProg;
  pni::pstd::autoRef< scene::uniform > mUniform00;
  pni::pstd::autoRef< scene::camera > mCam;

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


scene::texture* loadCubemap ( std::string const& rootPath )
{
  scene::texture* pTex = new scene::texture;
  pTex->setTarget( scene::texture::CubeMapTarget );

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
  
  pni::pstd::searchPath spath;
  spath.addPath(rootPath);
  
  uint32_t cubeSide = 0;
  for ( auto iter : imgs )
  {
    if ( spath.resolve(iter, iter) )
    {
      pTex->setImage(
          img::factory::getInstance().loadSync(iter),
          static_cast< scene::texture::ImageId > ( cubeSide++ ));
    }
    else
      PNIDBGSTR("could not resolve cube map texture image file");
  }
  
  return pTex;
}

scene::geom* buildQuad ()
{
  using namespace scene;

  geom* pMainGeom = new geom;

  geomData* pGeomData = new geomData;
  pGeomData->attributesOp().push_back( { CommonAttributeNames[ geomData::Position], geomData::Position, geomData::DataType_FLOAT, geomData::PositionComponents } );
//  pGeomData->attributesOp().push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );
  pGeomData->attributesOp().push_back ( { CommonAttributeNames[ geomData::TCoord00], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );
//  pGeomData->attributesOp().push_back ( { CommonAttributeNames[ geomData::TCoord01], geomData::TCoord00, geomData::DataType_FLOAT, geomData::TCoord00Components } );

  pGeomData->resizeTrisWithCurrentAttributes(4, 2);
  
  float QuadSize = 0.5f;
  
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
  
  prog* pProg = new prog;
  
  pProg->setProgStr(prog::Vertex, R"(
      attribute vec4 a_position;
      attribute vec2 a_uv00;

      varying lowp vec4 v_color;
      varying lowp vec2 v_uv00;

      uniform mat4 u_mvpMat;

      void main()
      {
          v_color = vec4(1.0,1.0,1.0,1.0);

          v_uv00 = a_uv00;

          gl_Position = u_mvpMat * a_position;
      }
  )");
  pProg->setProgStr(prog::Fragment, R"(
      varying lowp vec4 v_color;
      varying lowp vec2 v_uv00;

      uniform sampler2D u_tex00;

      void main()
      {
        lowp vec4 tex00 = texture2D ( u_tex00, v_uv00 );
        gl_FragColor = tex00 * v_color;
      }
  )");
  
  return pProg;
}

- (void) setupScene
{
  using namespace scene;

  GLKView *view = (GLKView *)self.view;

    // h & w swapped for landscape
  img::base::Dim width  = (img::base::Dim)view.drawableHeight;  // self.view.frame.size.height * self.view.contentScaleFactor;
  img::base::Dim height = (img::base::Dim)view.drawableWidth; // self.view.frame.size.width * self.view.contentScaleFactor;

    ////////////////////////////////////////////////
    // set up main scene... will just be a quad with a rt texture
  img::base* pMainSceneImg = new img::base;
  pMainSceneImg->setSize(width, height, width);
  pMainSceneImg->setFormat(img::base::RGB565);

  texture* pMainSceneTex = new texture;
  pMainSceneTex->setName("tex scene dest");
  pMainSceneTex->setImage(pMainSceneImg);
  pMainSceneTex->setWrap(texture::ClampToEdge, texture::ClampToEdge, texture::ClampToEdge);

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

    // Path to the app bundle to get the test file.
  std::string bdir ( getShellPath(BundleDir) );
  std::string fname = { bdir + "/" + "test-00b.ase" };
//  std::string fname = { bdir + "/" + "cyclorama-00a.dae" };
//  std::string fname = { bdir + "/" + "ld-pod-01b-hi-icon.dae" };

    // Load the file, grab its bounding sphere so we can push back the camera an
    // appropriate amount.
  mLoadFuture = loader::factory::getInstance().loadAsync(fname);

  mRoot = new scene::group ();

    // Add default vert and frag programs
  mProg = new scene::prog;
  mProg->setDefaultProgs();
  mRoot->setState(mProg.get(), scene::state::Prog);

    // Create a default depth state object.
  scene::depth* pDepth = new scene::depth ();
  mRoot->setState(pDepth, scene::state::Depth);
  
  scene::texture* pCubemap = loadCubemap ( bdir );
  pCubemap->setOverride(true);
  mRoot->setState(pCubemap, scene::state::Texture01);
  
    // Create and setup the camera.
  mCam = new scene::camera ();
  mCam->setColorClear( { 0.0f, 0.2f, 0.0f, 1.0f } );
  mCam->setNormalizeMode( scene::camera::Normalize );
  mCam->setViewport( 0.0f, 0.0f, width, height );

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
  tspec.mWidth                          = width;
  tspec.mHeight                         = height;
  self->mTexSink->mFramebuffer->setColorTextureTarget(0, pMainSceneTex);
  self->mTexSink->mFramebuffer->setName("render to texture scene");

  self->mMainSink->addChild(self->mTexSink.get());
}

- (void) finalizeScene
{
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
  rot += 2.0f;

  pni::math::vec3 axis { 0.25f, 0.5f, 1.0f };
  axis.normalize();

  if ( mFile )
    mFile->matrixOp().setRot(rot, axis);
  
  mMainGeom->matrixOp().setRot(rot, axis);

    // Invoke the rendering pass.
  scene::renderSinkDd* rsDd = new scene::renderSinkDd;
  rsDd->startGraph(mMainSink.get(), 0.0);
}


@end
