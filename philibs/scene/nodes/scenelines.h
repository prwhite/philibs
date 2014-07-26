// ///////////////////////////////////////////////////////////////////
//
//  class: lines
//
// ///////////////////////////////////////////////////////////////////

#ifndef scenelines_h
#define scenelines_h

// ///////////////////////////////////////////////////////////////////

#include "scenegeomfx.h"
#include "scenedata.h"
#include "sceneuniform.h"
#include "scenedirty.h"
#include "pnivec2.h"

// ///////////////////////////////////////////////////////////////////

namespace scene {
    
// ///////////////////////////////////////////////////////////////////

class lines;
class lineData;
class lineStyle;

/**
  The lineDataBase class is for internal use of the #lineData classe.
  The types created in this "namespace" are available in both the #lineData scope.
*/

struct lineDataBase {
    enum SemanticTypes {
      Position,   // vec3
      Color,      // vec4
      Thickness,  // float
      MiterStyle  // uint8_t  ... but this should be padded out to 4 bytes
    };
    enum DataTypes {
      Float,
      Uint32
    };
      /// @warning: Unimplemented
    enum MiterStyles {
      None,
      Extension,
      Rounded
    };

  protected:
    lineDataBase () = default; // Can't instantiate this directly.
};

/**
  The line data class contains both the line values and the line primitive
  lengths list (hence, using dataIndexed).  So, after setting proper bindings,
  a lineData instance should #resize to indicate both the number of indipendent
  vertices, as well as the number of unique primitives (i.e., picking up the
  pen between separate line segements).  Then, the values stored in the index
  list should be the number of verts used for each primitive respectively.
  E.g., 3 line segments with 3 verts each would need the following init:
  @code 
    pLineData->mBinding.pushBack ( { ... } )  // add a binding for vert positions
    pLineData->resize ( 9, 3 );
    pLineData->getIndices () = { 3, 3, 3 };
  @endcode
*/

class lineData :
  public lineDataBase,
  public pni::pstd::refCount,
  public dirtyProp< lineData, lineData >,
  public dataIndexed< basicBinding< basicBindingItem<lineDataBase::SemanticTypes, lineDataBase::DataTypes>>>
{
  public:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const {}
  
};

// ///////////////////////////////////////////////////////////////////

/**
  The lineStyle class determines visual style of a given #lines instance.
  This includes how edges and dashes are anti-aliased, and whether dash
  use is enabled, etc.
*/


struct lineStyle :
  public dirtyProp< lineStyle, lineStyle >
{
  lineStyle () = default;
  
    /// @group Used to sharpen/soften edges for non-textured lines.
  float mEdgeMiddle = 0.2f;         /// Middle of antialiasing range for edge
  float mEdgeRange = 0.2f;          /// Falloff of antialiasing range for edge
  
    /// @group Used if mEnableFlags & Dash00 for non-textured lines.
  float mDashMiddle = 0.2f;         /// Middle of antialiasing range for dash
  float mDashRange = 0.025f;        /// Falloff of antialiasing range for dash
  float mDashPeriod = 0.1f;         /// "Length" of dash, 1 is len = thickness
  float mDashPhase = 0.0f;          /// Offset of dashes in u direction
  
    /// @group Used if mEnableFlags & Tex00, turns other parameters off
  float mTexMiddle = 0.5f;          /// Middle of antialiasing range for edge
  float mTexRange = 0.5f;           /// Falloff of antialiasing range for texture
  float mTexUMult = 1.0f;           /// Multiply u coord by this amount

    /// @group To drop pixels below an alpha threshold.
  float mAlphaRef { 0.1f };         /// Discard threshold
  
  enum EnableFlags
  {
    None = 0x00,
    Dash00 = 0x01,                  /// Enable dashed line strategy 00 (simple)
    Tex00 = 0x02                    /// Enable use of texture 0 as line fragment source.
                                    /// Setting this will change attached prog object src text
  };
  
  uint32_t mEnableFlags { None }; // Turn shader on/off
};

// ///////////////////////////////////////////////////////////////////

/**
  The lines class is specialized to draw a set of line primitives, based on
  the line points in the #lineData member, the #lineStyle and the internal
  uniform object that tracks graphic engine settings.  Each #lines instance
  can draw multiple continuous lines based on how the #lineData indices are
  specified.
  @warning Using the lineStyle::Tex00 enable flag will cause the lines internals
  to change the attached prog object source text.  If this prog object is aliased,
  then other nodes will be affected too.
  @warning The heuristic used to re-write the prog source text for Tex00 is not
  that robust.  If you want to switch a line from textured back to non-textured,
  it's possible that you might need to manually reset the program source text.
*/

class lines :
  public scene::geomFx
{
  public:
    using style = lineStyle;
  
    using LineDataRef = pni::pstd::autoRef<lineData>;
    using UniformRef = pni::pstd::autoRef<uniform>;
  
    virtual void update ( graphDd::fxUpdate const& update ) override;
    void updateTest ( graphDd::fxUpdate const& update );

      /// @group Easy accessors for dirty property types
      /// Ex:
      /// @code
      ///   pLines->lineDataProp().op().resize (...)
      /// @endcode
    void setLineData ( lineData* pData ) { mLineData = pData; setGeomBoundsDirty(); }
    lineData* getLineData () { return mLineData.get (); }
    lineData const* getLineData () const { return mLineData.get (); }
  
    void setLineStyle ( lineStyle const& style ) { mLineStyle = style; }
    lineStyle& getLineStyle () { return mLineStyle; }
    lineStyle const& getLineStyle () const { return mLineStyle; }
  
    void setLineUniform ( uniform* pUniform ) { mUniform = pUniform; }
    uniform* getLineUniform () { return mUniform.get(); }
    uniform const* getLineUniform () const { return mUniform.get(); }
  
      /// Set this if your destination framebuffer has non square pixels...
      /// e.g., rendering in a framebuffer that will be stretched non-
      /// uniformly when finally composed.
    void setViewportSizeRatio ( pni::math::vec2 const& val )
      { mVpSizeRatio = val; mUniform->setDirty(); }
    pni::math::vec2 const& getViewportSizeRatio () const { return mVpSizeRatio; }
    
  protected:
    void rebuildLines ();
    void rebuildUniform ();
    float preCalcLength ();
  
  private:
  
    LineDataRef mLineData;
    UniformRef mUniform { new uniform };
    lineStyle mLineStyle;
    pni::math::vec2 mVpSizeRatio { 1.0f, 1.0f };

    // From geomFx, geom
  public:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const
      {
        refs.push_back(mLineData.get());
        refs.push_back(mUniform.get());
      }
    virtual lines* dup () const override { return new lines ( *this ); }

  protected:
    virtual void generateGeomBounds () const override;



};

// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

  // Create a specialization for hashing lineData::SemanticTypes
  // Necessitated by SCENEDATABINDINGSARECACHED optimization in scene::data's
  // related basicBinding class.
namespace std
{
  template<>
  struct hash<scene::lineData::SemanticTypes>
  {
    size_t operator () ( scene::lineData::SemanticTypes val ) const
    {
      return std::hash< int > () ( val );
    }
  };
}

#endif // scenelines_h
