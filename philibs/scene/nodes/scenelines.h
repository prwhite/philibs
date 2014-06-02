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
  
    struct style
    {
      style () {}
    
      pni::math::vec2 mEdgeRange { 0.5f, 0.15f };         // { middle alpha, range }
      pni::math::vec4 mDashRange { 0.1f, 0.05f, 30.0f, 0.0f };  // { middle alpha, range, period/length, phase/offset }
      float mAlphaRef { 0.1f };                           // Discard threshold
      uint32_t mDashEnable { 1 };                         // Turn dashes on/off
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

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const
        { refs.push_back(mLineData.get()); refs.push_back(mUniform.get()); }
  
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
