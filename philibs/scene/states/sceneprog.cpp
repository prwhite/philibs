/////////////////////////////////////////////////////////////////////
//
//    file: sceneprog.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneprog.h"

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

      attribute vec4 position;
      attribute vec3 normal;

      varying lowp vec4 colorVarying;

      uniform mat4 modelViewProjectionMatrix;
      uniform mat3 normalMatrix;

      void main()
      {
          vec3 eyeNormal = normalize(normalMatrix * normal);
          vec3 lightPosition = vec3(0.0, 0.0, 1.0);
          vec4 diffuseColor = vec4(0.1, 0.1, 1.0, 1.0);
          
          float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));
                       
          colorVarying = diffuseColor * nDotVP;
          
          gl_Position = modelViewProjectionMatrix * position;
      }
    )" );

  setProgStr( Fragment, R"(
    #extension GL_EXT_separate_shader_objects : enable

    varying lowp vec4 colorVarying;

    void main()
    {
        gl_FragColor = colorVarying;
    }
  )" );
}

/////////////////////////////////////////////////////////////////////
// overrides from state


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


