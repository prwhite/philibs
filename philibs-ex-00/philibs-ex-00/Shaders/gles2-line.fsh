// ///////////////////////////////////////////////////////////////////
//  Fragment Program
// ///////////////////////////////////////////////////////////////////

precision lowp float;

varying lowp vec4 v_color;
varying lowp vec2 v_uv00;
uniform sampler2D u_tex00;

void main()
{
  float alpha = v_uv00[ 1 ];
  alpha *= alpha;
  alpha = 1.0 - alpha;
  
    // Externalize one or two of these as uniforms for fs line thickness
  const float range = 0.15;
  const float bottom = 0.5 - range;
  const float top = 0.5 + range;

    // Externalize as alpha-ref
  const highp float Cutoff = 0.1;
  
  highp float up = 1.0;

#define SINSTYLE
#ifdef SINSTYLE

    // Externalize as dash length
  const highp float mod = 30.0;
  up = sin ( v_uv00[ 0 ] * mod );
  up = up * 0.5 + 0.5;  // get in range [0,1]
  
  const highp float Min = 0.25;
  const highp float Max = 0.35;
  
  up = smoothstep ( Min, Max, up );

#endif // SINSTYLE
  
  alpha = smoothstep ( bottom, top, alpha );
  
  alpha *= v_color.a * up;
  
  if ( alpha < Cutoff ) discard;

  gl_FragColor = vec4 ( v_color.xyz, alpha );
}
