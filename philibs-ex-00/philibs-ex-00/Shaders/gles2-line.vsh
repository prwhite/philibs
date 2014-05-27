// ///////////////////////////////////////////////////////////////////
//  Vertex Program
// ///////////////////////////////////////////////////////////////////

precision lowp float;
attribute vec4 a_position;
attribute vec4 a_normal;
attribute vec4 a_color;
attribute vec2 a_uv00;

varying lowp vec4 v_color;
varying lowp vec2 v_uv00;

uniform mat4 u_mvpMat;
uniform mat3 u_normMat;

void main()
{
  v_uv00 = a_uv00;

    // get normal in ndc... it's still parallel to line segment though
  lowp vec3 norm = u_normMat * a_normal.xyz;
  
    // get normal to actually be perp to line segment
  norm = cross ( norm.xyz, vec3 ( 0, 0, 1 ) );

  gl_Position = u_mvpMat * a_position + vec4 ( norm, 1 );
  
  v_color = a_color;
  v_uv00 = a_uv00;
}
