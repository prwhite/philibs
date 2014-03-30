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

namespace
{
  static std::string unitTable[] =
  {
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15"
  };
};

void prog::setDefaultProgs ()
{
  std::string defines;
  for ( auto flag : mFlagVec )
  {
    switch ( flag.mFlag )
    {
      case Tex:
        defines += "#define UV" + unitTable[ flag.mUnit ];
        break;
      case Uv:
        defines += "#define UV" + unitTable[ flag.mUnit ];
        break;
      case UvReflection:
        defines += "#define UVREFLECTION" + unitTable[ flag.mUnit ];
        break;
      case Sampler2D:
        defines += "#define SAMPLER2D" + unitTable[ flag.mUnit ];
        break;
      case SamplerCube:
        defines += "#define SAMPLERCUBE" + unitTable[ flag.mUnit ];
        break;
      case Color:
        defines += "#define COLOR";
        break;
      case Normal:
        defines += "#define NORMAL";
        break;
      case DefaultLight:
        defines += "#define DEFAULTLIGHT";
        break;
    }
    defines += "\n";
  }

  setProgStr( Vertex,
    defines +
    R"(
      attribute vec4 a_position;
      attribute vec3 a_normal;

#ifdef COLOR
      attribute vec4 a_color;
#endif

#ifdef UV00
      attribute vec2 a_uv00;
      varying lowp vec2 v_uv00;
#endif // UV00

#ifdef UV01
      attribute vec2 a_uv01;
      varying lowp vec2 v_uv01;
#endif // UV01

      varying lowp vec4 v_color;
      varying lowp vec3 v_halfVector;
      varying lowp vec3 v_normal;
      varying lowp vec3 v_lvvec;

      uniform mat4 u_mvpMat;
      uniform mat3 u_normMat;

      void main()
      {
          vec3 lightPosition = vec3(10.0, 10.0, 40.0);    // should be a uniform

#ifdef COLOR
          v_color = a_color;
#else
          v_color = vec4(1.0, 1.0, 1.0, 1.0);   // should be a uniform
#endif // COLOR

#ifdef UV00
          v_uv00 = a_uv00;
#endif // UV00
#ifdef UV01
          v_uv01 = a_uv01;
#endif // UV01

          lowp vec4 pos = u_mvpMat * a_position;
          v_lvvec = normalize(lightPosition - pos.xyz);

          v_normal = normalize(u_normMat * a_normal);;
          v_halfVector = normalize(v_normal + lightPosition);

          gl_Position = u_mvpMat * a_position;
      }
    )" );

  setProgStr( Fragment,
    defines +
    R"(
      varying lowp vec4 v_color;
#ifdef UV00
      varying lowp vec2 v_uv00;
#endif // UV00
#ifdef UV01
      varying lowp vec2 v_uv01;
#endif // UV01
      varying lowp vec3 v_halfVector;
      varying lowp vec3 v_normal;
      varying lowp vec3 v_lvvec;

#ifdef SAMPLER2D00
      uniform sampler2D u_tex00;
#endif // SAMPLER2D00
#ifdef SAMPLER2D01
      uniform sampler2D u_tex01;
#endif // SAMPLER2D01


#ifdef SAMPLERCUBE00
      uniform samplerCube u_tex00;
#endif // SAMPLERCUBE00
#ifdef SAMPLERCUBE01
      uniform samplerCube u_tex01;
#endif // SAMPLERCUBE01

      void main()
      {
        lowp vec3 normalizedNormal = normalize ( v_normal );
      
          // Ambient
        lowp vec4 amb = vec4 ( 0.2, 0.2, 0.2, 1.0 );  // should be a uniform

          // Diffuse
        lowp vec4 diff  = vec4(max(0.0,dot(normalizedNormal,normalize(v_lvvec))) * v_color.rgb,1.0);
        
#ifdef SAMPLER2D00
        lowp vec4 tex00 = texture2D ( u_tex00, v_uv00 );
        diff *= tex00;
#endif // SAMPLER2D00
#ifdef SAMPLER2D01
        lowp vec4 tex01 = texture2D ( u_tex01, v_uv01 );
        diff *= tex01;
#endif // SAMPLER2D01

        //vec3 reflectedDirection = reflect(viewDirection, normalize(normalDirection));
        //gl_FragColor = textureCube(_Cube, reflectedDirection);
#ifdef SAMPLERCUBE00
#ifdef UVREFLECTION00
        lowp vec3 cubevec00  = reflect ( vec3 ( 0, 0, -1 ), normalizedNormal );
#else
        // TODO: Need other cube vector lookup functions
#endif // UVREFLECTION00
        lowp vec4 tex00 = textureCube ( u_tex00, cubevec00, 0.0 );
        diff *= tex00;
#endif // SAMPLERCUBE00

#ifdef SAMPLERCUBE01
#ifdef UVREFLECTION01
        lowp vec3 cubevec01  = reflect ( vec3 ( 0, 0, -1 ), normalizedNormal );
#else
        // TODO: Need other cube vector lookup functions
#endif // UVREFLECTION01
        lowp vec4 tex01 = textureCube ( u_tex01, cubevec01, 0.0 );
        diff *= tex01;
#endif // SAMPLERCUBE01

          // Specular
        highp float Shininess = 128.0;                 // should be a uniform
        lowp float specMod = max(0.0,dot(normalizedNormal,normalize(v_halfVector)));
        lowp vec4 spec = vec4 ( v_color.xyz * pow(specMod,Shininess), 1.0 );
//        spec *= tex00 * tex00 * tex00;  // cheesy effect to get spec modulated by texture.

        gl_FragColor = min(vec4(1.0,1.0,1.0,1.0), amb + diff + spec);

        gl_FragColor.a = v_color.a;
        
#ifdef TEX00
        gl_FragColor.a *= tex00.a;
#endif // TEX00
#ifdef TEX01
        gl_FragColor.a *= tex01.a;
#endif // TEX01

      }
    )" );
}

/////////////////////////////////////////////////////////////////////
// overrides from state


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


