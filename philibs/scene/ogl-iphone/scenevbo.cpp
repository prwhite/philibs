/////////////////////////////////////////////////////////////////////
//
//  file: scenevbo.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PNIPSTDLOGDISABLE

#include "pnilog.h"

#include "scenevbo.h"
#include "sceneprogobj.h"

#include "sceneogl.h"
#include <OpenGLES/ES2/glext.h>

#include <iostream>

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

/////////////////////////////////////////////////////////////////////

void vbo::bind ( geomData const* pData, progObj const* pProgObj )
{

    // Decent VAO example here:
    // http://www.lastrayofhope.com/tag/gl_oes_vertex_array_object/

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
//      case geomData::DataType_UNSIGNED_INT_2_10_10_10_RE: return GL_UNSIGNED_INT_2_10_10_10_REV;
    default:
    {
      PNIDBGSTR("attribute data type not supported");
      return GL_FLOAT;
    }
  }
}

void vbo::config ( geomData const* pData, progObj const* pProgObj )
{
  if ( ! pData )
    return;

  geomData::bindings const& bindings = pData->getBindings();

  if(pData->getDirty() || bindings.getDirty())
  {
    bind ( pData, pProgObj );
    glBindBuffer( GL_ARRAY_BUFFER, mAttrId );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndId);

#define PHINOBUFFERSUBDATA true

    if ( PHINOBUFFERSUBDATA )
    {
      glBufferData(GL_ARRAY_BUFFER, pData->getValueSizeBytes (), &( pData->getValues()[ 0 ] ), GL_DYNAMIC_DRAW );
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, pData->getIndexSizeBytes(), &( pData->getIndices()[ 0 ]), GL_DYNAMIC_DRAW);
    }
    else
    {
      glBufferData(GL_ARRAY_BUFFER, pData->getValueSizeBytes (), 0, GL_DYNAMIC_DRAW );
      glBufferSubData(GL_ARRAY_BUFFER, 0, pData->getValueSizeBytes(), & ( pData->getValues()[ 0 ] ));

      glBufferData(GL_ELEMENT_ARRAY_BUFFER, pData->getIndexSizeBytes(), 0, GL_DYNAMIC_DRAW);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, pData->getIndexSizeBytes(), &( pData->getIndices()[ 0 ]));
    }

CheckGLError

      // Now set up attribute pointers.
    for ( auto const& bindingIter : bindings )
    {
      GLint attribId = glGetAttribLocation(pProgObj->getVertexProgHandle(), bindingIter.mName.c_str());
CheckGLError
      if ( attribId != -1 ) // Ignore when attribute isn't present in shader.
      {
        glEnableVertexAttribArray(attribId);
CheckGLError
//        glVertexAttribPointer(attribId, bindingIter.mComponents, dataTypeToGlDataType(bindingIter.mDataType), GL_FALSE, bindings.getValueStrideBytes(), pValues + bindings.getValueOffset(bindingIter.mType));
        glVertexAttribPointer(attribId, bindingIter.mComponents, dataTypeToGlDataType(bindingIter.mDataType), GL_FALSE, bindings.getValueStrideBytes(),  reinterpret_cast< void* >( bindings.getValueOffsetBytes(bindingIter.mType)));
CheckGLError
      }
    }

    pData->clearDirty();
    pData->getBindings().clearDirty();

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


