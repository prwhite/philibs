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

    // get "normal" in clip... it's still tangent to line segment though
  float lNormLen = length ( a_normal.xyz );   // local len before matrix mult...
                                              // but meant to be used in screen pixels

  vec4 cSrc = u_mvpMat * a_position;
  vec4 lDst = a_position;
  vec4 lTangent = vec4 ( a_normal.xyz / lNormLen, 0.0 );
     // HACK: Keep tangent vectors that point toward the eye from projecting
     // through the COP, causing generated points to be on the wrong side of
     // the centerline.  Could be a uniform that defaults to something reasonable.
     // TODO: This should probably be proportional to the distance to the near clipping
     // plane.
  lTangent *= 0.01;
  
    // Offset local position by tangent
  lDst += lTangent;
  
    // Get tangent end in clip space
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
  nNorm.xy *= ( lNormLen * u_vpSizeRatio ) / u_vpSize;

    // Now add the calculated norm to the point on the line
  nSrc.xyz += nNorm;

    // Z offset toward camera to make lines in crotches stand out... but we
    // also do polygon offset in fixed function to make sure fragments move
    // toward the camera.
    // TODO: Scale this based on dz, rather than absolute value.
  float offset = -pow( 2.0, 8.0 ) / 65536.0;
  nSrc[ 2 ] += offset;

  gl_Position = nSrc;
  
  v_color = a_color;

#ifdef DOTEXTURE00
    // TODO: Expose 5.0 as uniform
  v_uv00 = vec2 ( a_uv00[ 0 ] * u_texRange[ 2 ], a_uv00[ 1 ] * 0.5 + 0.5 );
  v_texRange = u_texRange.xy;
#else
  v_uv00 = a_uv00;
#endif // DOTEXTURE00
  
}
