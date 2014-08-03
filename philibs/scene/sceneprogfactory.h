/////////////////////////////////////////////////////////////////////
//
//  class header: progFactory
//
/////////////////////////////////////////////////////////////////////

#ifndef progfactory_h
#define progfactory_h

/////////////////////////////////////////////////////////////////////

#include "scenegeom.h"
#include "sceneprog.h"
#include "pnithreadpool.h"
#include "pnisearchpath.h"

#include <map>
#include <functional>

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

  /** Creates and caches program strings that statically optimize for
      a given combination of lights/textures/etc.
      @note This is just sketched in right now, and generally unimplemented.
      @note Read the previous note.
  */

class progFactory
{
  public:
  
      /// Acquire the singleton for this class.
    static progFactory& getInstance ();

      /// Types of lighting.  Appropriate values for color, direction, etc.
      /// are set up with corresponding uniforms.
      /// @note Corresponding uniforms have not been defined.
    enum LightTypes
      {
        NoLight,
        Directional,
        Point,
//        Spot,      // Not supported yet

        MaxNumLightUnits = 8
      };
  
      /// Types specify whether texture units are enabled, and what kind of
      /// sampler and function they are using.
      /// Appropriate values for how the combination is applied are set
      /// with corresponding uniforms.
      /// @note Corresponding uniforms have not been defined.
    enum TextureTypes
      {
        NoTexture,
        Tex2D,
        CubemapReflection,
        CubemapRefraction
      };
  
      /// Ops specify how texture stages are combined...
      /// Appropriate values for how the combination is applied are set
      /// with corresponding uniforms.
      /// @note Corresponding uniforms have not been defined.
    enum TextureOps
      {
        Multiply,
        Decal,
        Add,
        Subtract,
        Divide,
        // Maybe others?  For SDFs?
      };
  
      // Specify enable/disable state and runtime properties for
      // a basic lighting and texturing shader pair.
    struct progProps
      {
          /// Set an entry to one of the enum values if a texture will
          /// be supplied for the texture unit.
          /// @attribute default NoTexture
        TextureTypes mTextureTypes[ scene::geomData::NumTexUnits ];

          /// Set an entry to one of the enum values if a texture and UVs will
          /// be supplied for the texture unit.
          /// @attribute default Multiply
        TextureOps mTextureOps[ scene::geomData::NumTexUnits ];
        
          /// From #LightTypes enum, either None for disabled, or
          /// one of the specific types.  Corresponding uniforms
          /// will need to be set.
          /// @attribute default NoLight
        LightTypes mLightUnitTypes[ MaxNumLightUnits ];
      };

      /// Generate and/or cache a program that matches the specified props.
    prog* genProg ( progProps const& props );
  
      /// Purge cache.  Will wipe any entry with only one reference (i.e., from
      /// this cache).
    void purgeCache ();
  
  
      /// @group load methods
    pni::pstd::searchPath mSearchPath;
  
    typedef std::future< prog* > ProgFuture;
    typedef std::vector< std::string > Filenames;

      /// @note order of filenames in string should be in order of
      /// prog::Stage enum... e.g., Vertex first, then Fragment, etc.
    prog* loadSync ( Filenames const& fnames );

      /// @note order of filenames in string should be in order of
      /// prog::Stage enum... e.g., Vertex first, then Fragment, etc.
    ProgFuture loadAsync ( Filenames const& fnames );

      /// @note Not implemented yet.
    void cancel ( ProgFuture const& progFuture );

  protected:

  private:
    typedef pni::pstd::autoRef< prog > ProgRef;
    typedef std::map< progProps, ProgRef > ProgMap;
  
//    ProgMap mProgMap; // TODO: Needs mutex when we start using it!!!
  
    pni::pstd::threadPool mThreadPool { 1 };
};

/////////////////////////////////////////////////////////////////////

} // end namespace scene

#endif // progfactory_h
