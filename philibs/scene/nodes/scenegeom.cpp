/////////////////////////////////////////////////////////////////////
//
//    file: scenegeom.cpp
//
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include "scenegeom.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void geomData::dbg ()
{
  using namespace std;

  cout << "geomData : " << this << endl;

  size_t stride = getValueStride ();

  for ( size_t num = 0; num < mValues.size () / stride; ++num )
  {
    size_t cur = num * stride;
    cout << " pos = " 
        << mValues[ cur     ] << ", "
        << mValues[ cur + 1 ] << ", "
        << mValues[ cur + 2 ] << endl;
    cur += 3;
    
    if ( mBindings & Normals )
    {
      cout << " norm = " 
          << mValues[ cur     ] << ", "
          << mValues[ cur + 1 ] << ", "
          << mValues[ cur + 2 ] << endl;
        cur += 3;
    }

    if ( mBindings & Colors )
    {
      cout << " color = " 
          << mValues[ cur     ] << ", "
          << mValues[ cur + 1 ] << ", "
          << mValues[ cur + 2 ] << ", "
          << mValues[ cur + 3 ] << endl;
        cur += 4;
    }

    if ( mBindings & TCoords0 )
    {
      cout << " uv0 = " 
          << mValues[ cur    ] << ", "
          << mValues[ cur + 1 ] << endl;
        cur += 2;
    }

    if ( mBindings & TCoords1 )
    {
      cout << " uv1 = " 
          << mValues[ cur    ] << ", "
          << mValues[ cur + 1 ] << endl;
        cur += 2;
    }
  }
  
  cout << " indices" << endl;
  for ( size_t num = 0; num < mIndices.size (); ++num )
  {
    cout << "  " << mIndices[ num ] << endl;
  }
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

 geom::geom() :
  mBoundsMode ( Default ),
  mGeomBoundsDirty ( true )
 {
   
 }

//geom::~geom()
//{
//    
//}

// geom::geom(geom const& rhs) :
//   node ( rhs ),
//   mBindings ( rhs.mBindings ),
//   mValues ( rhs.mValues ),
//   mIndices ( rhs.mIndices )
// {
//   
// }

//geom& geom::operator=(geom const& rhs)
//{
//    node::operator = ( rhs );
//    return *this;
//}

//bool geom::operator==(geom const& rhs) const
//{
//    return false;
//}

/////////////////////////////////////////////////////////////////////

void geom::setGeomBoundsDirty ()
{
  node::setBoundsDirty ();
  mGeomBoundsDirty = true;
}

/////////////////////////////////////////////////////////////////////

void geom::updateBounds () const
{
  if ( mBoundsMode == ForceEmpty )
  {
    if ( ! mBounds.isEmpty () )
    {
      mBounds.setEmpty ();
    }      
  }
  else
  {
    if ( ( ( mBoundsMode & Static ) && mBounds.isEmpty () )
        || ( ( mBoundsMode & Dynamic ) && mGeomBoundsDirty ) )
    {
      generateGeomBounds ();
      
      if ( mBoundsMode & Partition )
        generateGeomPartition ();
    }
  }

  mBounds.setIsDirty ( false );
  mGeomBoundsDirty = false;
}

/////////////////////////////////////////////////////////////////////

void geom::generateGeomBounds () const
{
  mBounds.setEmpty ();

  if ( ! mGeomData )
    return;

  if ( mGeomData->getElemCount () == 0 )
    return;

//printf ( "generate geom bounds for %x w/name %s\n", this, this->getName ().c_str () );

  Values const& values = mGeomData->getValues ();

  float const* end = &values.back ();
  SizeType incr = mGeomData->getValueStride ();
  for ( float const* ptr = &values[ 0 ]; 
    ptr < end;
    ptr += incr )
  {
    mBounds.extendBy ( ptr[ 0 ], ptr[ 1 ], ptr[ 2 ] );
  }
}

void geom::generateGeomPartition () const
{
  // TODO
}

/////////////////////////////////////////////////////////////////////

void geom::uniquifyGeometry ()
{
  if ( mGeomData )
  {
    if ( mGeomData->getNumRefs () > 1 )
      mGeomData = new geomData ( *mGeomData.get () );
  }
}

/////////////////////////////////////////////////////////////////////

void geom::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  node::collectRefs ( refs );
  refs.push_back ( mGeomData.get () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace scene 


