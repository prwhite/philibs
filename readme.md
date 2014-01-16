
# PHILIBS

## A C++, OpenGL ES scene graph library for iOS

This library supports 3D rendering of a 'scene' described in memory as a '[scene graph](http://en.wikipedia.org/wiki/Scene_graph)', using the GLES graphics library for hardware accelerated rendering.

### Current features include:

* Hierarchical scene management, ray-based scene picking with culling, multi-texture, GLES state management, linear algebra library, particle system, sprites, and more.

### Half-ass'd features include:

* Programmable pipeline support is in, and relatively generalized, but I need to build up more high-level concepts around it, including a lighting model and shaders that are conditional on light and texture binding, etc.

* Key-frame animation system, UI system, bitmap font node, deformable height-mapped terrain, spatialized sound, 2d image and 3d file format loaders, view frustum culling

In many cases, these half-ass'd features are built around proper abstractions allowing future extensibility, but tend to lack a wide range of implementations and implementation details.  E.g., an image loading abstraction exists, but the DDS image loader doesn't currently support compressed textures, and a PVR loader has not been implemented based on this abstraction yet.

### Really missing:

* Parallelism/thread safety strategy
* A ton of other things

### Short-term feature goals:

* A number of features to support a high-polish 3D authoring application, such as real-time reflections, soft shadows, section planes, high-quality line rendering, ambient occlusion, etc.

## Status

A _version_ of this code has been used in an iOS title [(LD50)](http://labs.prehiti.com/ld50/).  But, this code has not been used in its current incarnation as a stand-alone library, _yet_.

2014/01/16

GLES2 support is working, albeit in its earliest stages.  The current default shader supports positions/normals/uv00 and a single texture.  It implements a basic per-pixel lighting model, but a number of the parameters for lighting are magic numbers in the shader, and need to be moved out to uniforms derived from scene graph state.  That's one of the next things.

2014/01/02

New work for GLES2 support is going on in the gles2 branch.  Currently, I am pinning down scene graph representations of programs, uniforms, and attributes.  This will allow a basic programmable pipeline in the short term, and it lays the groundwork for user-defined attributes in a future release.

2014/01/20

The library now builds as a relatively clean 32 and 64 bit binary for iOS 7.  A basic example app is also provided.  This work is available on the master branch.

2013/12/01

This current goal of this project is to transform the code into a proper stand-alone library.  Following this transformation, the library will be upgraded from support of GLES 1.x to _only_ support GLES 2.x.






