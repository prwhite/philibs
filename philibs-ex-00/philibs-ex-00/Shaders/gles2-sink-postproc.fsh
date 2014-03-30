precision lowp float;
varying vec2 v_uv00;

uniform sampler2D u_tex00;

#define BLOOM

void calcLuminanceBloom ( inout vec4 color )
{
  const float div = 1.0 / 3.0;
  float ret = color.r * div;
  ret += color.g * div;
  ret += color.b * div;
  
  ret *= ret;
//  ret *= ret;
//  ret *= ret;
  
  color = vec4 ( ret, ret, ret, 1.0 );
}

void main()
{
#ifdef BLOOM
  vec4 tex01 = texture2D ( u_tex00, v_uv00, 4.5 );
  calcLuminanceBloom ( tex01 );

  vec4 tex02 = texture2D ( u_tex00, v_uv00, 7.5 );
  calcLuminanceBloom ( tex02 );
  
  tex01 += tex02;
  tex01 *= 0.5;
#endif // BLOOM

  vec4 tex00 = texture2D ( u_tex00, v_uv00 );
#ifdef BLOOM
  gl_FragColor = min ( vec4 ( 1.0, 1.0, 1.0, 1.0 ), tex00 + tex01  );
#else
  gl_FragColor = tex00;
#endif // BLOOM
}


