//
//  phiViewController.m
//  philibs-ex-00
//
//  Created by Payton White on 12/13/13.
//  Copyright (c) 2013 PREHITI HEAVY INDUSTRIES. All rights reserved.
//

#import "phiViewController.h"

#include "philibs/sceneloaderase.h"
#include "philibs/sceneddogl.h"
#include "philibs/scenecamera.h"
#include "philibs/scenegroup.h"
#include "philibs/pnisphere.h"
#include "philibs/scenedepth.h"
#include "philibs/scenelighting.h"
#include "philibs/scenelight.h"
#include "philibs/scenecull.h"
#include "philibs/scenelightpath.h"

#include "philibs/sceneprog.h"
#include "philibs/sceneuniform.h"

#include "philibs/pnimathstream.h"
#include "philibs/scenedbgdd.h"

#include "philibs/sceneloaderfactory.h"

#include "pniosxplatform.h"

#include <iostream>
#include <fstream>

@interface phiViewController () {
  
  scene::ddOgl* mDd;
  pni::pstd::autoRef< scene::node > mRoot;
  pni::pstd::autoRef< scene::node > mFile;
  pni::pstd::autoRef< scene::prog > mProg;
  pni::pstd::autoRef< scene::uniform > mUniform00;
  pni::pstd::autoRef< scene::camera > mCam;
  loader::factory::LoadFuture mLoadFuture;
  
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;
- (void)initScene;

@end

@implementation phiViewController

- (void)viewDidLoad
{
  [super viewDidLoad];
  
  self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  
  if (!self.context) {
    NSLog(@"Failed to create ES context");
  }
  
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

- (void)setupGL
{
  [EAGLContext setCurrentContext:self.context];

    // Path to the app bundle to get the test file.
  std::string bdir ( getShellPath(BundleDir) );
//  std::string fname = { bdir + "/" + "test-00c.ase" };
//  std::string fname = { bdir + "/" + "cyclorama-00a.dae" };
  std::string fname = { bdir + "/" + "ld-pod-01b-hi-icon.dae" };

    // Load the file, grab its bounding sphere so we can push back the camera an
    // appropriate amount.
  mLoadFuture = loader::factory::getInstance().loadAsync(fname);

    // Create the GL visitor and the root group.
  mDd = new scene::ddOgl ();
  mRoot = new scene::group ();

    // Add default vert and frag programs
  mProg = new scene::prog;
  mProg->setDefaultProgs();
  mRoot->setState(mProg.get(), scene::state::Prog);

    // Create a default depth state object.
  scene::depth* pDepth = new scene::depth ();
  mRoot->setState(pDepth, scene::state::Depth);

    // Create and setup the camera.
  mCam = new scene::camera ();
  mCam->setColorClear( pni::math::vec4 ( 0.1f, 0.1f, 0.1f, 1.0f ) );
  mCam->setNormalizeMode( scene::camera::Normalize );
  mCam->setViewport( 0.0f, 0.0f, self.view.frame.size.height * self.view.contentScaleFactor, self.view.frame.size.width * self.view.contentScaleFactor );

  scene::cull* pCull = new scene::cull;
  mRoot->setState(pCull, scene::state::Cull);

  mRoot->addChild ( mCam.get () );

    // Set the root -> camera path as the sink (viewer) for the scene.
  scene::nodePath camPath ( mCam.get () );
  mDd->setSinkPath ( camPath );
}

- (void) initScene
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
      [self initScene];
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
    
    // Invoke the rendering pass.
  mDd->startGraph ( mRoot.get () );
}


@end
