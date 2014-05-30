// ///////////////////////////////////////////////////////////////////
//  Vertex Program
// ///////////////////////////////////////////////////////////////////

  // TODO: Re-specify precision everywhere
  // TODO: Optimize from vector to scalar where possible
  // TODO: Consider moving thickness to user-defined vert attribute...
  //        Might save some cycles both on CPU and GPU, or not.
  // TODO: Use defines to turn on/off color per-point and globally
  // TODO: Use defines to turn on/off thickness per-point and globally
  // TODO: support uniforms:
    // Destination viewport size (if rendering to texture, might be diff)
    // Global color
    // Global thickness

precision highp float;

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
//#define INCLIP
#ifdef INCLIP
    // This method of generating lines works in clip space, which causes
    // artifacts due to perspective shear of corner points.  It's basically
    // unusable, but kept here for reference... for now.
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
#endif // USESCALEFACTOR

#define USEVPSIZE
#ifdef USEVPSIZE
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

#endif // INCLIP

#define INNDC
#ifdef INNDC
    // This method does line extrusion in ndc, after the perspective divide.

    // prefixes: c = clip, n = ndc, e = eye, l = scene local
  v_uv00 = a_uv00;

    // get normal in clip... it's still parallel to line segment though
  float lNormLen = length ( a_normal.xyz );   // local len before matrix mult...
                                              // but meant to be used in screen pixels

  vec4 cSrc = u_mvpMat * a_position;
  vec4 lDst = a_position;
  vec4 lTangent = vec4 ( a_normal.xyz / lNormLen, 0.0 );
  lDst += lTangent;
  vec4 cDst = u_mvpMat * lDst;
  
    // Save w for points so we can put things back in clip coords later
  float cSrcW  = cSrc.w;
  float cDstW  = cDst.w;
  
    // Do perspective divide to get into ndc
  vec4 nSrc = cSrc;
  vec4 nDst = cDst;
  nSrc /= cSrcW;
  nDst /= cDstW;

    // Calc actual normal in ndc
  vec3 nTangent = nDst.xyz - nSrc.xyz;
  vec3 nNorm = cross ( nTangent, vec3 ( 0.0, 0.0, 1.0 ) );  // cross w z-out to get perp vector in ndc
  nNorm = normalize ( nNorm );

    // Cheesy viewport conversion
    // Temporary hardcode before we commit to uniforms for this
  const float widthTarget = 2048.0;
  const float heightTarget = 1536.0;
  const float widthSource = 2048.0;
  const float heightSource = 1024.0;
  
    // TODO: Make these uniforms as a vec2 u_vpRatio
  const float xfactor = widthSource / widthTarget;
  const float yfactor = heightSource / heightTarget;

    // Make the ndc-based normal the right length in pixels by scaling by
    // the original length divided by forward diff of the pixel size in ndc units.
  nNorm.x *= lNormLen * xfactor;
  nNorm.y *= lNormLen * yfactor;
  nNorm.xy /= u_vpSize;
  nNorm.z = 0.0;        // should be redundant after cross with z-out

    // Now add the calculated norm to the point on the line
  nSrc.xyz += nNorm;

    // Put output pos back in clip space
  cSrc = nSrc * cSrcW;

  gl_Position = cSrc;
  
  v_color = a_color;
  v_uv00 = a_uv00;
#endif // INNDC
}
