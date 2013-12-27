/////////////////////////////////////////////////////////////////////
//
//  file: scenevbo.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PNIPSTDLOGDISABLE

#include "pnilog.h"

#include "scenevbo.h"

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

void vbo::bind ( geomData const* pData )
{

    // Decent VAO example here:
    // http://www.lastrayofhope.com/tag/gl_oes_vertex_array_object/

//  glBindVertexArrayOES(mArrObjId);
  glBindBuffer( GL_ARRAY_BUFFER, mAttrId );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndId);
}

/////////////////////////////////////////////////////////////////////

void vbo::config ( geomData const* pData )
{
  bind ( pData );
  
  if ( ! pData )
    return;
  
CheckGLError

  glBufferData(GL_ARRAY_BUFFER, pData->getValueSizeBytes (), &( pData->getValues()[ 0 ] ), GL_DYNAMIC_DRAW );

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, pData->getIndexSizeBytes(), &( pData->getIndices()[ 0 ]), GL_DYNAMIC_DRAW);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void vbo::init ()
{
  glGenBuffers ( 1, &mAttrId );
  glGenBuffers ( 1, &mIndId );
//  glGenVertexArraysOES ( 1, &mArrObjId );
}

void vbo::clear ()
{
  glDeleteBuffers ( 1, &mAttrId );
  glDeleteBuffers ( 1, &mIndId );
//  glDeleteVertexArraysOES( 1, &mArrObjId );
}

/////////////////////////////////////////////////////////////////////
// overrides from attr


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


