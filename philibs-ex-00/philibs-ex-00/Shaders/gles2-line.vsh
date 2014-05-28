// ///////////////////////////////////////////////////////////////////
//  Vertex Program
// ///////////////////////////////////////////////////////////////////

  // TODO: specify precision everywhere
  // TODO: Optimize from vector to scalar where possible
  // TODO: Consider moving thickness to user-defined vert attribute...
  //        Might save some cycles both on CPU and GPU, or not.
  // TODO: Use defines to turn on/off color per-point and globally
  // TODO: Use defines to turn on/off thickness per-point and globally
  // TODO: support uniforms:
    // Destination viewport size (if rendering to texture, might be diff)
    // Global color
    // Global thickness

attribute vec4 a_position;
attribute vec4 a_normal;
attribute vec4 a_color;
attribute vec2 a_uv00;

varying lowp vec4 v_color;
varying lowp vec2 v_uv00;

uniform mat4 u_mvpMat;
uniform mat3 u_normMat;
uniform vec2 u_vpSize;

void main()
{
  v_uv00 = a_uv00;

    // get normal in clip... it's still parallel to line segment though
  float origLen = length ( a_normal.xyz );          // local len before matrix mult
  vec3 norm = u_normMat * a_normal.xyz;             // norm is in clip
  
    // get normal to actually be perp to line segment and screen normal
  norm = cross ( norm, vec3 ( 0, 0, 1 ) );          // in clip
  norm = normalize ( norm );
  
// #define USESCALEFACTOR
#ifdef USESCALEFACTOR
  float vpSizeFactor = ( u_vpSize[ 0 ] + u_vpSize[ 1 ] ) / 2.0;
  norm.x *= origLen / vpSizeFactor; // u_vpSize[ 1 ];
  norm.y *= origLen / vpSizeFactor; // u_vpSize[ 0 ];
#else
    // Make the ndc-based normal the right length in pixels by scaling by
    // the original length divided by forward diff of the pixel size in ndc units.
  norm.x *= origLen / u_vpSize[ 0 ];
  norm.y *= origLen / u_vpSize[ 1 ];
#endif // USESCALEFACTOR

    // Adjust pos with normal in ndc, so manually convert in and out of clip space
  vec4 tpos = u_mvpMat * a_position;
  float w = tpos[ 3 ];
  tpos /= w;
  tpos.xyz += norm;
  tpos *= w;

  gl_Position = tpos;
  
  v_color = a_color;
  v_uv00 = a_uv00;
}
