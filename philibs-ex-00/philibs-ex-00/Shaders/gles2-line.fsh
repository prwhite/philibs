// ///////////////////////////////////////////////////////////////////
//  Fragment Program
// ///////////////////////////////////////////////////////////////////

precision lowp float;

varying lowp vec4 v_color;
varying lowp vec2 v_uv00;
varying lowp vec2 v_texRange;
varying lowp float v_thickness;

uniform sampler2D u_tex00;
uniform lowp vec2 u_edgeRange;
uniform highp vec4 u_dashRange; // needs highp for [2] and [3]
uniform lowp float u_alphaRef;
uniform lowp int u_dashEnable;

void main()
{
    // Calc alpha for edges based on v coord
  float alpha = v_uv00[ 1 ];
  vec4 color = v_color;

#ifndef DOTEXTURE00

// #define QUADRATIC
#ifdef QUADRATIC
  alpha *= alpha;   // from [-1,1] to [1,1] through zero
#else // triangle wave
  alpha = abs ( alpha );
#endif // QUADRATIC
  
    // MAYBE: Add uniform that determines if/how we do this... i.e., a scalar for v direction
    // this will allow us to do double/triple stroke lines, etc.
  alpha = 1.0 - alpha;  // [0,0] through one now
  
    // to do line thickness/antialising fidelity, the top and bottom of the smoothstep
#define AUTOEDGERANGE
#ifdef AUTOEDGERANGE
    // Auto edge range uses half of the size of one pixel (eventually +/- that
    // amount), derived from the varying thickness.
    // So... 1.99 is crazy-ish... don't want to round up to two which would make
    // 3 pixel blurry in some orientations.
  float erange = 1.99 / v_thickness;
  float ecenter = erange;     // to move away from exact tri edge
#else
  float ecenter = u_edgeRange[ 0 ];
  float erange = u_edgeRange[ 1 ];
#endif // AUTOEDGERANGE
  
  float bottom = ecenter - erange;
  float top = ecenter + erange;

  alpha = smoothstep ( bottom, top, alpha );
  
#define DODASHES
#ifdef DODASHES
      // Do dashes, with antialiasing of the dash break edges using u coord
  if ( u_dashEnable != 0 )
  {
    highp float up = 1.0;

      // Use triangle wave func to give us low and high points based on u coord
      // that we can pick out with smoothstep to get longer/shorter dashes.
      // u_dashRange[ 2 ] changes overall period and u_dashRange[ 3 ] changes phase.
    up = abs ( mod ( v_uv00[ 0 ] / u_dashRange[ 2 ] + u_dashRange[ 3 ], 2.0 ) - 1.0 );

      // apply upper lower bounds for smoothstep
    up = smoothstep ( u_dashRange[ 0 ] - u_dashRange[ 1 ], u_dashRange[ 0 ] + u_dashRange[ 1 ], up );

    alpha *= up;
  }

#endif // DODASHES

#else // DOTEXTURE00
  
  vec4 tex = texture2D ( u_tex00, v_uv00 );
  alpha = smoothstep ( v_texRange[ 0 ] - v_texRange[ 1 ], v_texRange[ 0 ] + v_texRange[ 1 ], tex.r );

#endif // DOTEXTURE00
  
  color.a *= alpha;
  
  if ( color.a < u_alphaRef ) discard;

  gl_FragColor = color;
}
