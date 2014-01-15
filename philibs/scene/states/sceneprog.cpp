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
      #extension GL_EXT_separate_shader_objects : enable

      attribute vec4 a_position;
      attribute vec3 a_normal;
      attribute vec2 a_uv00;
      attribute vec2 a_uv01;

      varying lowp vec4 v_colorVarying;
      varying lowp vec2 v_uv00;
      varying lowp vec2 v_uv01;

      uniform mat4 u_mvpMat;
      uniform mat3 u_normMat;

      void main()
      {
          vec3 eyeNormal = normalize(u_normMat * a_normal);
          vec3 lightPosition = vec3(0.0, 0.0, 1.0);
          vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
          
          float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));
                       
          v_colorVarying = diffuseColor * nDotVP;
          
          gl_Position = u_mvpMat * a_position;
          v_uv00 = a_uv00;
          v_uv01 = a_uv01;
      }
    )" );

  setProgStr( Fragment,
    R"(
      #extension GL_EXT_separate_shader_objects : enable

      varying lowp vec4 v_colorVarying;
      varying lowp vec2 v_uv00;
      varying lowp vec2 v_uv01;

      uniform sampler2D u_tex00;
      uniform sampler2D u_tex01;

      void main()
      {
        lowp vec4 color = v_colorVarying;
        color *= texture2D ( u_tex00, v_uv00 );
        gl_FragColor = color;
      }
    )" );
}

/////////////////////////////////////////////////////////////////////
// overrides from state


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


