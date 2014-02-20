precision lowp float;
attribute vec4 a_position;
attribute vec2 a_uv00;

varying vec2 v_uv00;

uniform mat4 u_mvpMat;

void main()
{
  v_uv00 = a_uv00;

  gl_Position = u_mvpMat * a_position;
}
