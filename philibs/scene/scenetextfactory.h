// ///////////////////////////////////////////////////////////////////
//
//  class: textFactory
//
// ///////////////////////////////////////////////////////////////////

#ifndef scenetextfactory_h
#define scenetextfactory_h

// ///////////////////////////////////////////////////////////////////

#include "fontfontfactory.h"
#include "scenetext.h"

#include "sceneprog.h"
#include "scenetexture.h"
#include "sceneuniform.h"
#include "sceneblend.h"

#include "pniautoref.h"

#include <unordered_map>

// ///////////////////////////////////////////////////////////////////

namespace scene {
  
// ///////////////////////////////////////////////////////////////////

class textFactory 
{
  public:
    textFactory();
//    virtual ~textFactory();
//    textFactory(textFactory const& rhs);
//    textFactory& operator=(textFactory const& rhs);
//    bool operator==(textFactory const& rhs) const;

    static textFactory& getInstance ();

      /// Will automatically set up a new text node with default shaders
      /// and uniforms.  These can be overriden by calling code by reassigning
      /// these states.
      /// @note Currently the uniforms are assigned to slot 00.  This is
      /// generally fine because text will not be lit and uses its own
      /// vert and frag shaders, etc., so there is
      /// no conflict with other uniforms in slot 00.
    text* newText ( font::font* pFont );
  
    void setDefaultProg ( prog* pProg ) { mDefProg = pProg; }
    prog* getDefaultProg () const { return mDefProg.get(); }
  
    void setDefUniform ( uniform* pUniform ) { mDefUniform = pUniform; }
    uniform* getDefaultUniform () const { return mDefUniform.get(); }
  
      /// This will purge a font's texture from the internal textFactory
      /// cache.
      /// @note This does not guarantee the texture will be deleted.  Other
      /// objects that have a reference will keep the texture in memory.
    void unloadFontTexture ( font::font* pFont )
      { mTextureMap.erase ( pFont->getTextureFname() ); }

    void setUniformDefaults ( float minVal, float maxVal, pni::math::vec4 const& color );

  protected:
    using ProgRef = pni::pstd::autoRef< prog >;
    using UniformRef = pni::pstd::autoRef< uniform >;
    using TextureRef = pni::pstd::autoRef< texture >;
    using BlendRef = pni::pstd::autoRef< blend >;
  
    using TextureMap = std::unordered_map< std::string, TextureRef >;
  
    ProgRef mDefProg;
    UniformRef mDefUniform;
    BlendRef mDefBlend;
    TextureMap mTextureMap;
    float mMinVal                     = 0.6f - 3.0f / 16.0f;
    float mMaxVal                     = 0.6f + 3.0f / 16.0f;
    pni::math::vec4 mColor            = { 0.0f, 0.0f, 0.0f, 1.0f };
  
    void initDefaults ();
  
  private:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenetextfactory_h
