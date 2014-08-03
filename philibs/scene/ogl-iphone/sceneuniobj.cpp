// ////////////////////////////////////////////////////////////////////////////
//
//  class: uniobj
//
// ////////////////////////////////////////////////////////////////////////////

#include "sceneuniobj.h"
#include <OpenGLES/ES2/glext.h>

// ///////////////////////////////////////////////////////////////////

namespace scene {
    
// ///////////////////////////////////////////////////////////////////

uniobj* uniobj::getOrCreate ( uniform const* pUni, progObj* pProgObj )
{
  if ( uniobj* pObj = pUni->getTravData< uniobj >(Draw) )
  {
    pObj->config ( pUni, pProgObj );
    return pObj;
  }
  else
  {
    pObj = new uniobj;
    pObj->config ( pUni, pProgObj );
    const_cast< uniform* > ( pUni )->setTravData ( Draw, pObj );
    return pObj;
  }
}

void uniobj::bind ()
{
  apply ();
  
    // TODO: When we have uniform buffer objects, we'll just bind that
}

void uniobj::apply ()
{
  for ( auto& binding : mBindings )
  {
    auto const& uBinding = *binding.mBinding;
    GLsizei count = ( GLsizei ) uBinding.getCount();
    switch ( uBinding.getType() )
    {
      case uniform::binding::Float1: glProgramUniform1fvEXT ( binding.mProgId, binding.mLoc, count, uBinding.getFloats() ); break;
      case uniform::binding::Float2: glProgramUniform2fvEXT ( binding.mProgId, binding.mLoc, count, uBinding.getFloats() ); break;
      case uniform::binding::Float3: glProgramUniform3fvEXT ( binding.mProgId, binding.mLoc, count, uBinding.getFloats() ); break;
      case uniform::binding::Float4: glProgramUniform4fvEXT ( binding.mProgId, binding.mLoc, count, uBinding.getFloats() ); break;

      case uniform::binding::Int1: glProgramUniform1ivEXT ( binding.mProgId, binding.mLoc, count, uBinding.getInts() ); break;
      case uniform::binding::Int2: glProgramUniform2ivEXT ( binding.mProgId, binding.mLoc, count, uBinding.getInts() ); break;
      case uniform::binding::Int3: glProgramUniform3ivEXT ( binding.mProgId, binding.mLoc, count, uBinding.getInts() ); break;
      case uniform::binding::Int4: glProgramUniform4ivEXT ( binding.mProgId, binding.mLoc, count, uBinding.getInts() ); break;

        // Note: Simulator was returning error 0x501 (invalid value) for GL_TRUE to transpose param.
        // No big though, we don't need it because our matrices are in the right format.
      case uniform::binding::Matrix2: glProgramUniformMatrix2fvEXT ( binding.mProgId, binding.mLoc, count, GL_FALSE, uBinding.getFloats() ); break;
      case uniform::binding::Matrix3: glProgramUniformMatrix3fvEXT ( binding.mProgId, binding.mLoc, count, GL_FALSE, uBinding.getFloats() ); break;
      case uniform::binding::Matrix4: glProgramUniformMatrix4fvEXT ( binding.mProgId, binding.mLoc, count, GL_FALSE, uBinding.getFloats() ); break;

      default:
        PNIDBGSTR ( "case not handled, getType out of range" );
        break;
    }
  }
}

void uniobj::config ( uniform const* pUni, progObj* pProgObj )
{
  if ( pUni->getDirty() || pProgObj != mLastProg )
  {
    mBindings.clear();
    
    for ( auto& uBinding : pUni->getBindings () )
    {
      GLuint progId = uBinding.second.getStage() == uniform::binding::Vertex ?
          pProgObj->getVertexProgHandle() : pProgObj->getFragmentProgHandle();
    
      GLint loc = glGetUniformLocation ( progId, uBinding.first.c_str() );
      uniform::binding const* pBinding = &uBinding.second;
      mBindings.push_back ( { loc, progId, pBinding } );
    }
    
    pUni->clearDirty ();
    mLastProg = pProgObj;
  }
}

// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

