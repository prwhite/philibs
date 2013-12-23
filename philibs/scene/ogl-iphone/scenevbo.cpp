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

void vbo::bind ( geom* const pGeom )
{
  glBindBuffer( GL_ARRAY_BUFFER, mAttrId );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndId);
}

/////////////////////////////////////////////////////////////////////

void vbo::config ( geom* const pGeom )
{
  bind ( pGeom );
  
  if ( ! pGeom )
    return;
  
  geomData* pGeomData = pGeom->getGeomData();
  
  if ( ! pGeomData )
    return;
  
CheckGLError

  glBufferData(GL_ARRAY_BUFFER, pGeomData->getValueSizeBytes (), &( pGeomData->getValues()[ 0 ] ), GL_DYNAMIC_DRAW );

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, pGeomData->getIndexSizeBytes(), &( pGeomData->getIndices()[ 0 ]), GL_DYNAMIC_DRAW);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void vbo::init ()
{
  glGenBuffers ( 1, &mAttrId );
  glGenBuffers ( 1, &mIndId );
}

void vbo::clear ()
{
  glDeleteBuffers ( 1, &mAttrId );
  glDeleteBuffers ( 1, &mIndId );
}

/////////////////////////////////////////////////////////////////////
// overrides from attr


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


