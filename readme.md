
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

## Example

Check out the source, then open, build, and run the project file `philibs-ex-00/philibs-ex-00.xcodeproj`.  You will generally see something spinning on screen... the current example is geared toward landscape orientation BTW.

## Status

A _version_ of this code has been used in an iOS title [(LD50)](http://labs.prehiti.com/ld50/).  But, this code has not been used in its current incarnation as a stand-alone library, _yet_.

2014/01/27

Merged assimp branch to master.  This includes the [Assimp](http://assimp.sourceforge.net/) library for 3D file importing.  Currently this is set up to load DAE, STL, OBJ, and PLY files.  Imported material colors are currently ignored (which matches the default shader functionality).  I'm keeping a fork of Assimp with a more up-to-date xcode project... I'll probably clean this part up and submit as a pull request.

2014/01/18

GLES2 support, from the gles2 branch, has been merged to master.  There are still a few things not buttoned up, like interpreting the scene graph light nodes as uniforms, etc.  That will come soon, ideally with sufficient generalization to allow for a range of lighting models.  Additionally, the file loader and image loaders have added factories that also intermediate asynchronous loading.  These use some C++11 features, such as threads/futures, etc.

2014/01/16

GLES2 support is working, albeit in its earliest stages.  The current default shader supports positions/normals/uv00 and a single texture.  It implements a basic per-pixel lighting model, but a number of the parameters for lighting are magic numbers in the shader, and need to be moved out to uniforms derived from scene graph state.  That's one of the next things.

2014/01/02

New work for GLES2 support is going on in the gles2 branch.  Currently, I am pinning down scene graph representations of programs, uniforms, and attributes.  This will allow a basic programmable pipeline in the short term, and it lays the groundwork for user-defined attributes in a future release.

2014/01/20

The library now builds as a relatively clean 32 and 64 bit binary for iOS 7.  A basic example app is also provided.  This work is available on the master branch.

2013/12/01

This current goal of this project is to transform the code into a proper stand-alone library.  Following this transformation, the library will be upgraded from support of GLES 1.x to _only_ support GLES 2.x.

## License

The philibs source code is covered by the [zlib/libpng](http://opensource.org/licenses/Zlib) open source license, except where otherwise noted.  E.g:

	Copyright (c) 2014 Payton R White
	
	This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
	
	Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
	
	1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
	
	2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
	
	3. This notice may not be removed or altered from any source distribution.



