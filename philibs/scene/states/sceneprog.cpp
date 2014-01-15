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

      varying lowp vec4 v_colorVarying;

      uniform mat4 u_mvpMat;
      uniform mat3 u_normMat;

      void main()
      {
          vec3 eyeNormal = normalize(u_normMat * a_normal);
          vec3 lightPosition = vec3(0.0, 0.0, 1.0);
          vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);
          
          float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));
                       
          v_colorVarying = diffuseColor * nDotVP;
          
          gl_Position = u_mvpMat * a_position;
      }
    )" );

  setProgStr( Fragment, R"(
    #extension GL_EXT_separate_shader_objects : enable

    varying lowp vec4 v_colorVarying;

    void main()
    {
        gl_FragColor = v_colorVarying;
    }
  )" );
}

/////////////////////////////////////////////////////////////////////
// overrides from state


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


