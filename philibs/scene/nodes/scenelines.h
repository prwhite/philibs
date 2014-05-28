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
  
    virtual void update ( graphDd::fxUpdate const& update ) override;
  
    dirty< pni::pstd::autoRef< lineData > > mLineData {
      0,
      [&] () { this->setGeomBoundsDirty(); },
      [&] () { this->rebuildLines(); }
    };

    dirty< pni::pstd::autoRef< uniform > > mUniform {
      new uniform,
      [&] () { this->rebuildUniform(); }
    };

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const
        { refs.push_back(mLineData.get()); refs.push_back(mUniform.get()); }
  
  protected:
    void rebuildLines ();
    void rebuildUniform ();
  
  private:

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
