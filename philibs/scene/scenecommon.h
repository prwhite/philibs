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
#include "sceneuniform.h"

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
static_assert ( sizeof ( CommonAttributeNames ) / sizeof ( CommonAttributeNames[ 0 ] ) == geomData::TCoord15 + 1,
    "CommonAttributeNames not in sync with geomData::AttributeType enum");

enum CommmonUniformIds : uint16_t
{
  UniformModelViewProjMatrix,
  UniformNormalMatrix,
  UniformViewportSize,
  UniformTex00,
  UniformTex01,
  UniformTex02,
  UniformTex03,
  UniformTex04,
  UniformTex05,
  UniformTex06,
  UniformTex07,
  UniformTex08,
  UniformTex09,
  UniformTex10,
  UniformTex11,
  UniformTex12,
  UniformTex13,
  UniformTex14,
  UniformTex15,
  
  UniformTextMin,
  UniformTextMax,
  UniformTextColor,
  
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
  "u_mvpMat",
  "u_normMat",
  "u_vpSize",
  "u_tex00",
  "u_tex01",
  "u_tex02",
  "u_tex03",
  "u_tex04",
  "u_tex05",
  "u_tex06",
  "u_tex07",
  "u_tex08",
  "u_tex09",
  "u_tex10",
  "u_tex11",
  "u_tex12",
  "u_tex13",
  "u_tex14",
  "u_tex15",
  
  "u_textMin",
  "u_textMax",
  "u_textColor"
};

static_assert ( ( sizeof ( CommonUniformNames ) / sizeof ( CommonUniformNames[ 0 ] ) ) == CommonUniformIdsCount, "foo" );

  // Whoa... this does a hard cast... can lead to buffer overruns...
  // don't do careless stuff with this.

  /// Get uniform storage as specified type.  Beware this includes a hard
  /// cast that could cause overruns, etc.
  /// There is /some/ protection for buffer overruns now, but still exercise care.
template< class Type >
Type&
commonUniformOp ( uniform* pUniform, CommmonUniformIds id )
{
  assert ( pUniform->uniformOp ( CommonUniformNames[ id ] ).getStorageSize() == sizeof ( Type ) );

  float* fp = pUniform->uniformOp ( CommonUniformNames[ id ] ).getFloats();
  Type& ret = * ( Type* ) ( fp );
  return ret;
}


/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

} // end namespace scene

#endif // pniscenecommon_h
