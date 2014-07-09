/////////////////////////////////////////////////////////////////////
//
//  file: scenevbo.cpp
//
/////////////////////////////////////////////////////////////////////

// Decent VAO examples/references:
// http://t-machine.org/index.php/2013/10/18/ios-open-gl-es-2-multiple-objects-at-once/
// http://www.lastrayofhope.com/tag/gl_oes_vertex_array_object/
// http://www.khronos.org/registry/gles/extensions/OES/OES_vertex_array_object.txt
// http://www.opengl.org/wiki/Vao#Vertex_Array_Object

/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PNIPSTDLOGDISABLE

#include "pnilog.h"
#include "pnidbg.h"

#include "scenevbo.h"
#include "sceneprogobj.h"

#include "sceneogl.h"
#include <OpenGLES/ES2/glext.h>

#include <iostream>

inline void const* BUFFER_OFFSET( size_t offset ) { return reinterpret_cast< void const* >( offset ); }

using namespace std;

/////////////////////////////////////////////////////////////////////

namespace scene {

/////////////////////////////////////////////////////////////////////

vbo::vbo():
  mAttrId ( 0 ),
  mIndId( 0 ),
  mArrObjId( 0 )
{
  init ();
}

vbo::~vbo()
{
  clear ();
}

// vbo::vbo(vbo const& rhs)
// {
//   init ();
// }

// vbo& vbo::operator=(vbo const& rhs)
// {
//   return *this;
// }
// 
// bool vbo::operator==(vbo const& rhs) const
// {
//
//   return false;
// }

vbo* vbo::getOrCreate ( geomData const* geomDataIn, progObj const* pProgObj )
{
    // get or create textureObject for this texture
  if ( vbo* pObj = static_cast< vbo* > ( geomDataIn->getTravData ( Draw ) ) )
  {
    pObj->config ( geomDataIn, pProgObj );
    return pObj;
  }
  else
  {
    pObj = new vbo;
    pObj->config ( geomDataIn, pProgObj );
    const_cast< geomData* > ( geomDataIn )->setTravData ( Draw, pObj );
    return pObj;
  }
}

/////////////////////////////////////////////////////////////////////

void vbo::bind ( geomData const* pData, progObj const* pProgObj )
{
  glBindVertexArrayOES(mArrObjId);
}

/////////////////////////////////////////////////////////////////////

GLuint dataTypeToGlDataType ( geomData::DataType dataType )
{
  switch (dataType) {
      case geomData::DataType_BYTE: return GL_BYTE;
      case geomData::DataType_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
      case geomData::DataType_SHORT: return GL_SHORT;
      case geomData::DataType_UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
      case geomData::DataType_INT: return GL_INT;
      case geomData::DataType_UNSIGNED_INT: return GL_UNSIGNED_INT;
      case geomData::DataType_FLOAT: return GL_FLOAT;
      case geomData::DataType_HALF_FLOAT: return GL_HALF_FLOAT_OES;
//      case geomData::DataType_DOUBLE: return GL_DOUBLE;
//      case geomData::DataType_INT_2_10_10_10_REV: return GL_INT_2_10_10_10_REV;
//      case geomData::DataType_UNSIGNED_INT_2_10_10_10_REV: return GL_UNSIGNED_INT_2_10_10_10_REV;
    default:
      PNIDBGSTR("attribute data type not supported");
      return GL_FLOAT;
  }
}

void vbo::config ( geomData const* pData, progObj const* pProgObj )
{
  if ( ! pData )
    return;

  geomData::Binding const& bindings = pData->mBinding;

  if(pData->dataProp().getDirty()) // NEWGEOM || attributes.getDirty())
  {
    bind ( pData, pProgObj );
    glBindBuffer( GL_ARRAY_BUFFER, mAttrId );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndId);

#define PHINOBUFFERSUBDATA true

    if ( PHINOBUFFERSUBDATA )
    {
      glBufferData(GL_ARRAY_BUFFER, pData->sizeBytes (), pData->getPtr< geomData::ValueType >(), GL_DYNAMIC_DRAW );
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, pData->sizeBytesIndices(), pData->getIndicesPtr(), GL_DYNAMIC_DRAW);
    }
    else
    {
      glBufferData(GL_ARRAY_BUFFER, pData->sizeBytes (), 0, GL_DYNAMIC_DRAW );
      glBufferSubData(GL_ARRAY_BUFFER, 0, pData->sizeBytes (), pData->getPtr< geomData::ValueType >() );

      glBufferData(GL_ELEMENT_ARRAY_BUFFER, pData->sizeBytesIndices(), 0, GL_DYNAMIC_DRAW);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, pData->sizeBytesIndices(), pData->getIndicesPtr());
    }

CheckGLError

      // Now set up attribute pointers.
//    for ( auto const& attributeIter : attributes )
    for ( auto const& binding : bindings )
    {
      GLint attribId = glGetAttribLocation(pProgObj->getVertexProgHandle(), binding.mName.c_str());
CheckGLError
      if ( attribId != -1 ) // Ignore when attribute isn't present in shader.
      {
        glEnableVertexAttribArray(attribId);
CheckGLError
        glVertexAttribPointer(attribId, (GLint) binding.mCount, dataTypeToGlDataType(binding.mDataType), GL_FALSE, (GLsizei) bindings.getValueStrideBytes(), BUFFER_OFFSET( bindings.getValueOffsetBytes(binding.mSemanticType)));
CheckGLError
      }
    }

    pData->dataProp().clearDirty();
    // NEWGEOM pData->getAttributes().clearDirty();

    glBindVertexArrayOES(0);  // Clear so other state doesn't leak into this VAO
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void vbo::init ()
{
  glGenBuffers ( 1, &mAttrId );
  glGenBuffers ( 1, &mIndId );
  glGenVertexArraysOES ( 1, &mArrObjId );
}

void vbo::clear ()
{
  glDeleteBuffers ( 1, &mAttrId );
  glDeleteBuffers ( 1, &mIndId );
  glDeleteVertexArraysOES( 1, &mArrObjId );
}

/////////////////////////////////////////////////////////////////////
// overrides from attr


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


