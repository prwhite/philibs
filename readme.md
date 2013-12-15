
# PHILIBS

**A C++, OpenGL ES scene graph library for iOS**

This library supports 3D rendering of a 'scene' described in memory as a '[scene graph](http://en.wikipedia.org/wiki/Scene_graph)'.

### Current features include:

* Hierarchical scene management, ray-based scene picking with culling, multi-texture, GLES state management, linear algebra library, particle system, sprites

### Half-ass'd features include:

* Key-frame animation system, UI system, bitmap font node, deformable height-mapped terrain, spatialized sound, 2d image and 3d file format loaders, view frustum culling

In many cases, these half-ass'd features are built around proper abstractions allowing future extensibility, but tend to lack a wide range of implementations and implementation details.  E.g., an image loading abstraction exists, but the DDS image loader doesn't currently support compressed textures, and a PVR loader has not been implemented based on this abstraction yet.

### Really missing:

* GLES 2.0 support (programmable pipeline), parallelism/thread safety strategy

### Short-term feature goals:

* A number of features to support a high-polish 3D authoring application, such as real-time reflections, soft shadows, section planes, high-quality line rendering, ambient occlusion, 

## Status

A _version_ of this code has been used in an iOS title [(LD50)](http://labs.prehiti.com/ld50/).  But, this code has not been used in its current incarnation as a stand-alone library.

This current goal of this project is to transform the code into a proper stand-alone library.  Following this transformation, the library will be upgraded from support of GLES 1.x to _only_ support GLES 2.x.






