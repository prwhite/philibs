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

struct lineNamespace {
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
  enum MiterStyles {
    None,
    Extension,
    Rounded
  };
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
  public lineNamespace,
  public pni::pstd::refCount,
  public dataIndexed< basicBinding< basicBindingItem<lineNamespace::SemanticTypes, lineNamespace::DataTypes>>>
{
  public:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const {}
  
};


class lines :
  public scene::geomFx
{
  public:
  
      // TODO: Change the values/names to something much closer to their semantic usage
    struct style
    {
      style () {}
      
      float mEdgeMiddle = 0.2f;         // Middle of antialiasing range for edge
      float mEdgeRange = 0.1f;          // Falloff of antialiasing range for edge
      
      float mDashMiddle = 0.2f;         // Middle of antialiasing range for dash
      float mDashRange = 0.1f;          // Falloff of antialiasing range for dash
      float mDashPeriod = 0.1f;         // "Length" of dash, 1 is len = thickness
      float mDashPhase = 0.0f;          // Offset of dashes in u direction

      float mAlphaRef { 0.1f };         // Discard threshold
      
      enum EnableFlags
      {
        None = 0x00,
        Dash00 = 0x01                   // Enable dashed line strategy 00 (simple)
      };
      
      uint32_t mEnableFlags { None }; // Turn shader on/off
    };
  
    virtual void update ( graphDd::fxUpdate const& update ) override;
    void updateTest ( graphDd::fxUpdate const& update );
  
      /// mLineData
    dirty< pni::pstd::autoRef< lineData > > mLineData {
      0,
      [&] () { this->setGeomBoundsDirty(); },
      [&] () { this->rebuildLines(); }
    };

      /// mUniform
    dirty< pni::pstd::autoRef< uniform > > mUniform {
      new uniform,
      [&] () { this->rebuildUniform(); }
    };
  
    dirty< size_t > mTest {
      0
    };
  
      /// mStyle
    dirty< style > mStyle {
      {},
      [&] () { this->rebuildStyle(); },
      [&] () {}
    };

      // Set this if your destination framebuffer has non square pixels...
      // e.g., rendering in a framebuffer that will be stretched non-
      // uniformly when finally composed.
    void setViewportSizeRatio ( pni::math::vec2 const& val )
      { mVpSizeRatio = val; mUniform->setDirty(); }
    pni::math::vec2 const& getViewportSizeRatio () const { return mVpSizeRatio; }
  
  protected:
    void rebuildLines ();
    void rebuildUniform ();
    void rebuildStyle ();
    float preCalcLength ();
  
  private:
  
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
