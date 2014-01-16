/////////////////////////////////////////////////////////////////////
//
//    file: sceneprog.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneprog.h"

#include "scenecommon.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

prog::prog()
{
  
}

prog::~prog()
{
  
}

// prog::prog(prog const& rhs)
// {
//   
// }
// 
// prog& prog::operator=(prog const& rhs)
// {
//   
//   return *this;
// }
// 
// bool prog::operator==(prog const& rhs) const
// {
//   
//   return false;
// }

void prog::setDefaultProgs ()
{
  setProgStr( Vertex,
    R"(
      attribute vec4 a_position;
      attribute vec3 a_normal;
      attribute vec2 a_uv00;
      attribute vec2 a_uv01;

      varying lowp vec4 v_color;
      varying lowp vec2 v_uv00;
      varying lowp vec2 v_uv01;
      varying lowp vec3 v_halfVector;
      varying lowp vec3 v_normal;
      varying lowp vec3 v_lvvec;

      uniform mat4 u_mvpMat;
      uniform mat3 u_normMat;

      void main()
      {
          vec3 lightPosition = vec3(10.0, 10.0, 40.0);    // should be a uniform
          vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);   // should be a uniform

          v_color = diffuseColor;

          v_uv00 = a_uv00;
          v_uv01 = a_uv01;

          lowp vec4 pos = u_mvpMat * a_position;
          v_lvvec = normalize(lightPosition - pos.xyz);

          v_normal = normalize(u_normMat * a_normal);;
          v_halfVector = normalize(v_normal + lightPosition);

          gl_Position = u_mvpMat * a_position;
      }
    )" );

  setProgStr( Fragment,
    R"(
      varying lowp vec4 v_color;
      varying lowp vec2 v_uv00;
      varying lowp vec2 v_uv01;
      varying lowp vec3 v_halfVector;
      varying lowp vec3 v_normal;
      varying lowp vec3 v_lvvec;

      uniform sampler2D u_tex00;
      uniform sampler2D u_tex01;

      void main()
      {
          // Ambient
        lowp vec4 amb = vec4 ( 0.2, 0.2, 0.2, 1.0 );  // should be a uniform

          // Diffuse
        lowp vec4 diff = vec4(max(0.0,dot(normalize(v_normal),normalize(v_lvvec))) * v_color.rgb,1.0);
        lowp vec4 tex00 = texture2D ( u_tex00, v_uv00 );
        diff *= tex00;

          // Specular
        lowp float Shininess = 128.0;                 // should be a uniform
        lowp float specMod = max(0.0,dot(normalize(v_normal),normalize(v_halfVector)));
        lowp vec4 spec = vec4 ( v_color.xyz * pow(specMod,Shininess), 1.0 );
        spec *= tex00 * tex00 * tex00;  // cheesy effect to get spec modulated by texture.

        gl_FragColor = min(vec4(1.0,1.0,1.0,1.0), amb + diff + spec);
      }
    )" );
}

/////////////////////////////////////////////////////////////////////
// overrides from state


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


