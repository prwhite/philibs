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
#include "philibs/scenelightpath.h"

#include "philibs/pnimathstream.h"

#include "pniosxplatform.h"

#include <iostream>

@interface phiViewController () {
  
  scene::ddOgl* mDd;
  pni::pstd::autoRef< loader::base > mLoader;
  pni::pstd::autoRef< scene::node > mRoot;
  pni::pstd::autoRef< scene::node > mFile;
  pni::pstd::autoRef< scene::camera > mCam;
  
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
  
  self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
  
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
  std::string fname = { bdir + "/" + "test-00.ase" };
  
    // Load the file, grab its bounding sphere so we can push back the camera an
    // appropriate amount.
  mLoader = new loader::ase;
  mFile = mLoader->load( fname );
  std::cout << "mFile bounds is " << mFile->getBounds() << std::endl;
  pni::math::sphere boundingSphere;
  boundingSphere.extendBy(mFile->getBounds());

    // Create the GL visitor and the root group.
  mDd = new scene::ddOgl ();
  mRoot = new scene::group ();
  
    // Create a default depth state object.
  scene::depth* pDepth = new scene::depth ();
  mRoot->setState(pDepth, scene::state::Depth);

    // Enable basic lighting state.
  scene::lighting* pLighting = new scene::lighting;
  pLighting->setAmbient ( pni::math::vec4 ( 0.2f, 0.2f, 0.2f, 1.0f ) );  // TEMP
  mRoot->setState ( pLighting, scene::state::Lighting );
  
    // Create and setup the camera.
  mCam = new scene::camera ();
  mCam->matrixOp().setTrans ( 0.0f, 0.0f, 1.5f * boundingSphere.getRadius() );
  mCam->setColorClear( pni::math::vec4 ( 0.1f, 0.1f, 0.1f, 1.0f ) );
  mCam->setNormalizeMode( scene::camera::Normalize );
  mCam->setViewport( 0.0f, 0.0f, self.view.frame.size.height * self.view.contentScaleFactor, self.view.frame.size.width * self.view.contentScaleFactor );

    // Create a dynamic light.
  scene::light* pLight = new scene::light;
  pLight->setDirection( pni::math::vec3 ( 1.0f, -1.0f, -1.0f ) ); // Not normalized :(
  
    // Add file and camera as children to the root.
  mRoot->addChild ( mCam.get () );
  mRoot->addChild ( mFile.get () );
  mRoot->addChild ( pLight );
  
    // Tell the root of the scene what lights affect it.
  scene::lightPath* pLightPath = new scene::lightPath ();
  pLightPath->setLight ( pLight, 0 );
  mRoot->setState ( pLightPath, scene::state::LightPath );
  
    // Set the root -> camera path as the sink (viewer) for the scene.
  scene::nodePath camPath ( mCam.get () );
  mDd->setSinkPath ( camPath );
}

- (void)tearDownGL
{
  [EAGLContext setCurrentContext:self.context];
  
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
  
  
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    // Very cheesy rotation animation.
  static float rot = 0.0f;
  rot += 2.0f;
  mFile->matrixOp().setRot(rot, 0.0f, 1.0f, 0.0f );
  
    // Invoke the rendering pass.
  mDd->startGraph ( mRoot.get () );
}


@end
