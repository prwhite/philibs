/////////////////////////////////////////////////////////////////////
//
//  file: sceneprogObj.cpp
//
/////////////////////////////////////////////////////////////////////

/* Resources for this implementation:

http://www.khronos.org/registry/gles/extensions/EXT/EXT_separate_shader_objects.txt
https://www.opengl.org/sdk/docs/man/xhtml/glBindProgramPipeline.xml
http://www.g-truc.net/post-0348.html
http://onlygraphix.com/2013/08/18/why-and-when-you-should-use-separate-shader-programs-in-opengl/
http://www.lastrayofhope.com/tag/gl_ext_separate_shader_objects/
http://www.rushdigital.co.nz/blog/2013/12/extseparateshaderobject-on-ios

*/


//#define PNIDBGDISABLE
#define PNIPSTDLOGDISABLE

#include "pnilog.h"

#include "sceneprogObj.h"
#include "sceneprog.h"

#include "sceneogl.h"
#include <OpenGLES/ES2/glext.h>

#include <iostream>

using namespace std;

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

progObj::progObj()
{
  init ();
}

progObj::~progObj()
{
  clear ();
}

// progObj::progObj(progObj const& rhs)
// {
//   init ();
// }

// progObj& progObj::operator=(progObj const& rhs)
// {
//   return *this;
// }
// 
// bool progObj::operator==(progObj const& rhs) const
// {
//
//   return false;
// }

/////////////////////////////////////////////////////////////////////

void progObj::bind ( prog const* pData )
{
  glBindProgramPipelineEXT(mPipeline);
}

/////////////////////////////////////////////////////////////////////

  // TODO: Implement a shader cache, so we get more value out of
  // shader re-use with separable programs.
  // TODO: Also recognize currently bound vert/frag/other shader
  // and do not rebind if the individual shader prog matches.
  // Both these points have up-stream ramifications... if all
  // this class knows about a shader is the program string, it will
  // be expensive to do comparisons.  Maybe separate states for each
  // program would have been a better match, although we'd still need
  // to find a way to combine them in a program pipeline on the gl side
  // of things.

bool checkProgramLink ( GLuint prog )
{
  GLint isLinked = 0;
  glGetProgramiv(prog, GL_LINK_STATUS, &isLinked);

  if( ! isLinked )
  {
    GLint size = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &size);

    GLchar *str = new GLchar[ size ];
    GLsizei sizeOut = 0;

    glGetProgramInfoLog(prog, size, &sizeOut, str);

    PNIDBGSTR(str);

    delete[] str;
    return false;
  }
  else
    return true;
}

bool checkPipelineLink ( GLuint pipe )
{
  glValidateProgramPipelineEXT( pipe );

  GLint status = 0;
  glGetProgramPipelineivEXT( pipe, GL_VALIDATE_STATUS, &status);
  if( status == GL_FALSE )
  {
    PNIDBGSTR( "failed to validate program pipeline" );

    GLint size = 0;
    glGetProgramPipelineivEXT(pipe, GL_INFO_LOG_LENGTH, &size);

    GLchar *str = new GLchar[ size ];
    GLsizei sizeOut = 0;

    glGetProgramPipelineInfoLogEXT(pipe, size, &sizeOut, str);

    PNIDBGSTR(str);

    delete[] str;
    return false;
  }
  else
    return true;
}

void progObj::config ( prog const* pData )
{
    // We don't bind here... we only need to bind when we are about to draw
    // because all of the seperable shader stuff takes a pipeline param.  Although,
    // http://www.g-truc.net/post-0348.html says we might need it actually.
    // Tested on WIP @ a1116d8, didn't seem to make a diff.

  if ( ! pData )
    return;

  if ( pData->getDirty() )
  {

      // TODO: Parameterize this, so we don't have dup'd code for vert and frag.
    if ( ! pData->getProgStr ( prog::Vertex ).empty () )
    {
      glDeleteProgram(mVertProg);

      char const* str = pData->getProgStr ( prog::Vertex ).c_str();
      mVertProg = glCreateShaderProgramvEXT(GL_VERTEX_SHADER, 1, &str );
      if ( checkProgramLink(mVertProg))
        glUseProgramStagesEXT( mPipeline, GL_VERTEX_SHADER_BIT_EXT, mVertProg );
CheckGLError
    }
    else
    {
      glUseProgramStagesEXT( mPipeline, GL_VERTEX_SHADER_BIT_EXT, 0 );
    }

CheckGLError

    if ( ! pData->getProgStr ( prog::Fragment ).empty () )
    {
      glDeleteProgram(mFragProg);

      char const* str = pData->getProgStr ( prog::Fragment ).c_str();
      mFragProg = glCreateShaderProgramvEXT(GL_FRAGMENT_SHADER, 1, &str );
      if ( checkProgramLink(mFragProg))
        glUseProgramStagesEXT( mPipeline, GL_FRAGMENT_SHADER_BIT_EXT, mFragProg );
CheckGLError
    }
    else
    {
      glUseProgramStagesEXT( mPipeline, GL_FRAGMENT_SHADER_BIT_EXT, 0 );
    }

CheckGLError

  checkPipelineLink( mPipeline );

CheckGLError

    pData->clearDirty();
  }
  // config prog pipelines, shaders
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void progObj::init ()
{
  glGenProgramPipelinesEXT(1, &mPipeline);
}

void progObj::clear ()
{
  glDeleteProgramPipelinesEXT(1,&mPipeline);
  mPipeline = 0;

  glDeleteProgram(mVertProg);
  mVertProg = 0;

  glDeleteProgram(mFragProg);
  mFragProg = 0;
}

/////////////////////////////////////////////////////////////////////
// overrides from attr


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


