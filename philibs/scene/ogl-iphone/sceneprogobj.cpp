/////////////////////////////////////////////////////////////////////
//
//  file: sceneprogObj.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
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

    if ( ! pData->getProgStr ( prog::Vertex ).empty () )
    {
      glDeleteProgram(mVertProg);

      char const* str = pData->getProgStr ( prog::Vertex ).c_str();
      mVertProg = glCreateShaderProgramvEXT(GL_VERTEX_SHADER, 1, &str );
      glUseProgramStagesEXT( mPipeline, GL_VERTEX_SHADER_BIT_EXT, mVertProg );
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
      glUseProgramStagesEXT( mPipeline, GL_FRAGMENT_SHADER_BIT_EXT, mFragProg );
    }
    else
    {
      glUseProgramStagesEXT( mPipeline, GL_FRAGMENT_SHADER_BIT_EXT, 0 );
    }

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


