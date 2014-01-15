////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pniscenecommon_h
#define pniscenecommon_h

////////////////////////////////////////////////////////////////////

#include <string>
#include "scenegeom.h"

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

  /**
    This array centralizes attribute names throughout the code.  They should only
    be used by indexing the geomData::AttributeType values (instead of direct string use).
    e.g., CommonAttributeNames[ geomData::Colors ].  Any change there should
    be reflected here.  Or, maybe, this should just be moved to that class.
    @note We use shorter names here than in the associated enums due to a somewhat
    misplaced sense that it will be cheaper to string match shorter names at
    runtime. 
    @note: Maybe we should add "attrib..." here too (?).
  */
char const* const CommonAttributeNames[]= {
  "a_position",
  "a_normal",
  "a_color",
  "a_uv00",
  "a_uv01",
  "a_uv02",
  "a_uv03",
  "a_uv04",
  "a_uv05",
  "a_uv06",
  "a_uv07",
  "a_uv08",
  "a_uv09",
  "a_uv10",
  "a_uv11",
  "a_uv12",
  "a_uv13",
  "a_uv14",
  "a_uv15"
};

  // Make sure we have the same number of elements here and in the AttributeType
  // enums [for builtins].
static_assert ( sizeof ( CommonAttributeNames ) / sizeof ( CommonAttributeNames[ 0 ] ) == geomData::TCoords15 + 1,
    "CommonAttributeNames not in sync with geomData::AttributeType enum");

enum CommmonUniformIds : uint16_t
{
  ModelViewProjMatrix,
  NormalMatrix,
  CommonUniformIdsCount
};

  /**
    This array centralizes uniform names throughout the code.  They should only
    be used by indexing the CommonAttribIds values (instead of direct string use).
    @note We use shorter names here than in the associated enums due to a somewhat
    misplaced sense that it will be cheaper to string match shorter names at
    runtime. 
  */
char const* const CommonUniformNames[]= {
  "u_modelViewProjectionMatrix",
  "u_normalMatrix"
};

static_assert ( ( sizeof ( CommonUniformNames ) / sizeof ( CommonUniformNames[ 0 ] ) ) == CommonUniformIdsCount, "foo" );

/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

} // end namespace scene

#endif // pniscenecommon_h
