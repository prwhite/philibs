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
varying lowp vec2 v_texRange; // middle and range, not mult which is used below

uniform mat4 u_mvpMat;
uniform mat3 u_normMat;
uniform vec2 u_vpSize;
uniform vec2 u_vpSizeRatio;
uniform vec3 u_texRange;

void main()
{
    // This does line extrusion in ndc, after the perspective divide.

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

    // Make the ndc-based normal the right length in pixels by scaling by
    // the original length divided by forward diff of the pixel size in ndc units.
  nNorm.xy *= u_vpSizeRatio / u_vpSize * lNormLen;

    // Now add the calculated norm to the point on the line
  nSrc.xyz += nNorm;

    // Put output pos back in clip space
  cSrc = nSrc * cSrcW;

  gl_Position = cSrc;
  
  v_color = a_color;

#ifdef DOTEXTURE00
    // TODO: Expose 5.0 as uniform
  v_uv00 = vec2 ( a_uv00[ 0 ] * u_texRange[ 2 ], a_uv00[ 1 ] * 0.5 + 0.5 );
  v_texRange = u_texRange.xy;
#else
  v_uv00 = a_uv00;
#endif // DOTEXTURE00
  
}
