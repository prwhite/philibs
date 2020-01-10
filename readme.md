
# PHILIBS

## A C++, OpenGL ES scene graph library for iOS

This library supports 3D rendering of a 'scene' described in memory as a '[scene graph](http://en.wikipedia.org/wiki/Scene_graph)', using the GLES graphics library for hardware accelerated rendering.

### Current features include:

* Hierarchical scene management, ray-based scene picking with culling, multi-texture, GLES state management, linear algebra library, particle system, sprites, extensible programmable pipeline, SDF texture-based text, high-quality antialiased lines, generalized framebuffer object management and rendering order evaluation, and more.

### Partial features include:

* Programmable pipeline support is in, and relatively generalized, but I need to build up more high-level concepts around it, including a lighting model and shaders that are conditional on light and texture binding, etc.

* Key-frame animation system, UI system, deformable height-mapped terrain, spatialized sound, 2d image and 3d file format loaders, view frustum culling

In many cases, these incomplete features are built around proper abstractions allowing future extensibility, but tend to lack a wide range of implementations and implementation details.  E.g., an image loading abstraction exists, but the DDS image loader doesn't currently support compressed textures, and a PVR is implemented but not fully vetted.

### Really missing:

* Parallelism/thread safety strategy.  This is evolving now though, largely based on C++11 mechanisms and thread pools used by everything that does I/O.
* A ton of other things.

### Short-term feature goals:

* A number of features to support a high-polish 3D authoring application, such as real-time reflections, soft shadows, section planes, high-quality line rendering, ambient occlusion, etc.

## Example

Check out the source, then open, build, and run the project file `philibs-ex-00/philibs-ex-00.xcodeproj`.  You will generally see something spinning on screen... the current example is geared toward landscape orientation BTW.

## Status

A _version_ of this code has been used in an iOS title [(LD50)](http://labs.prehiti.com/ld50/).  But, this code has not been used in its current incarnation as a stand-alone library, _yet_.

##### 2014/07/14

Radically refactored `scene::geom` and `scene::lines` to re-use a common set of code for managing their raw data and the binding information that associates them with graphics library semantics (e.g., positions, normals, etc.).  Code for `scene::geomData` went to almost nothing, and now has a much better iteration interface.  E.g:

    void func ( geomData* pData )
    {
    	auto posEnd = pData->end< vec3 > ( geomData::Position );
    	for ( auto posCur = pData->begin< vec3 > ( geomData::Position ); posCur < posEnd; ++posCur )
    		*posCur *= 2.0f;	// Scale position by 2.0
    }

The `geomData::Position` semantic could indicate storage anywhere in the interleaved vertex attributes, as opposed to the position data needing to be at a specific or pre-defined offset in the vertex layout.  This extends to all semantics, including user-defined vertex attributes.

Just about every part of the system that uses these classes became much more self-documenting and much more concise (which is awesome).
    

##### 2014/06/05

Recent feature additions include:

* Added antialiased 3D line drawing via [`scene::lines`](https://github.com/prwhite/philibs/blob/master/philibs/scene/nodes/scenelines.h).  Lines are drawn in 3D, but pseudo-projected to screen space to have a uniform screen-space line-thickness (even when rendering into a non-square-pixel texture before final composition).  Lines are configurable in a number of ways, including per-point thickness and color, as well as global fragment processing directives like how fast the alpha should fall off on line edges and whether the line is dashed, and what the dash pattern looks like.
* Scene infrastructure: Added [`scene::data`](https://github.com/prwhite/philibs/blob/master/philibs/scene/scenedata.h) class to encapsulate the common pattern that is used by `geomData`, `lineData`, `uniforms`, `particles` and `sprites` for storing packed verts/points/attributes which are driven by a set of bindings (i.e. variant-record) and striding through the data with convenient iterators, a la STL.  Also added [`scene::dirty`](https://github.com/prwhite/philibs/blob/master/philibs/scene/scenedirty.h), which encapsulates the recurring property-like pattern of member access with callbacks to the owning class.  Unique about this class is the inherent lazy-cleanup logic, which means that a value will be invalidated on any mutation, but won't be re-created as valid until accessed for use.  This pattern is used on things like `geomData` to update bounding boxes only when necessary, and is now implemented using the `scene::dirty` class in `scene::lines`.  Next up is to refactor other classes that use this pattern in an ad hoc fashion to the common implementation. 
* Geometry: Added vert sharing and soft-shading normal generation to scene::geom.
* Text: Added UTF8->UCS2 conversion just before scene::text glyphs are generated.  All text rendering is now *much more* i18n-friendly
* Math: Cleaned up some linear algebra pni::math routines, including getting Euler-related functions consistent with z-out and adding plane and matrix mirror routines.

##### 2014/04/22

Recent feature additions include:

* SDF (signed distance field) texture-based text.  Currently only using one texture page and limited to ASCII.  Would not be much of a stretch to move to `std::wstring`... but a bit harder to actually put in the texture page logic.
* Evolving support for conditional compilation of shader text.  A set of flags have been introduced for the current default programs, and this mechanism extends generally to any program text.  Use the methods `scene::prog::setFlag` and `scene::prog::applyFlags`.
* Added additional factories for shader program I/O and font file I/O.  Note `progFactory` is still WIP.  All factories _mostly_ adhere to a common interface (though this is not through inheritance).  This means all factories have two loading methods, `loadSync` and `loadAsync`.  Some of the factories implement an additional caching layer, which is reflected in the class documentation.
* Fully generalized FBO (framebuffer object) management.  FBO objects are managed in a graph of `scene::renderSink`s that implies their rendering order.  Thus, framebuffers that generate textures for later passes are properly evaluated in order.  The current example program sets up a simple framebuffer post-process to generate a subtle glow around lighting highlights.
* Added texture cube map support.  Realtime cubemaps should work with the new FBO management system, but has not been tested.

##### 2014/01/27

Merged assimp branch to master.  This includes the [Assimp](http://assimp.sourceforge.net/) library for 3D file importing.  Currently this is set up to load DAE, STL, OBJ, and PLY files.  Imported material colors are currently ignored (which matches the default shader functionality).  I'm keeping a fork of Assimp with a more up-to-date Xcode project... I'll probably clean this part up and submit as a pull request.

##### 2014/01/18

GLES2 support, from the gles2 branch, has been merged to master.  There are still a few things not buttoned up, like interpreting the scene graph light nodes as uniforms, etc.  That will come soon, ideally with sufficient generalization to allow for a range of lighting models.  Additionally, the file loader and image loaders have added factories that also intermediate asynchronous loading.  These use some C++11 features, such as threads/futures, etc.

##### 2014/01/16

GLES2 support is working, albeit in its earliest stages.  The current default shader supports positions/normals/uv00 and a single texture.  It implements a basic per-pixel lighting model, but a number of the parameters for lighting are magic numbers in the shader, and need to be moved out to uniforms derived from scene graph state.  That's one of the next things.

##### 2014/01/02

New work for GLES2 support is going on in the gles2 branch.  Currently, I am pinning down scene graph representations of programs, uniforms, and attributes.  This will allow a basic programmable pipeline in the short term, and it lays the groundwork for user-defined attributes in a future release.

##### 2014/01/20

The library now builds as a relatively clean 32 and 64 bit binary for iOS 7.  A basic example app is also provided.  This work is available on the master branch.

##### 2013/12/01

This current goal of this project is to transform the code into a proper stand-alone library.  Following this transformation, the library will be upgraded from support of GLES 1.x to _only_ support GLES 2.x.

## License

The philibs source code is covered by the [zlib/libpng](http://opensource.org/licenses/Zlib) open source license, except where otherwise noted.  E.g:

	Copyright (c) 2014 Payton R White
	
	This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
	
	Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
	
	1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
	
	2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
	
	3. This notice may not be removed or altered from any source distribution.



