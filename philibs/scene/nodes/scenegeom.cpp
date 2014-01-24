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

void geomData::dbg ( std::ostream& ostr ) const
{
  using namespace std;

  ostr << "geomData : " << this << endl;

  size_t stride = getAttributes ().getValueStride ();
  size_t numVerts = mValues.size () / stride;
  size_t cur = 0;

  ostr << " attributes: num verts = " << numVerts <<
      " num values = " << mValues.size() <<
      " stride = " << stride << std::endl;
  ostr << " indices: num indices = " << mIndices.size () << endl;

  for ( size_t num = 0; num < numVerts; ++num )
  {
    for ( auto attrIter : mAttributes )
    {
      ostr << num << " " << attrIter.mName << " = ";

      for ( size_t comp = 0; comp < attrIter.mComponents; ++comp )
        ostr << mValues[ cur++ ] << " ";
    }
    ostr << endl;
  }

  for ( size_t num = 0; num < mIndices.size (); ++num )
    ostr << "  " << num << " " << mIndices[ num ] << endl;
}

void geomData::updateBounds () const
{
  mBounds.setEmpty ();
  
  if ( getIndexCount () == 0 )
    return;
  
  Values const& values = getValues ();
  
  float const* end = &values.back ();
  SizeType incr = mAttributes.getValueStride ();
  for ( float const* ptr = &values[ 0 ];
       ptr < end;
       ptr += incr )
  {
    mBounds.extendBy ( ptr[ 0 ], ptr[ 1 ], ptr[ 2 ] );
  }
  
  mBounds.setIsDirty( false );
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
//   mAttributes ( rhs.mAttributes ),
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
  if ( mGeomData )
    mGeomData->setBoundsDirty();
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
  if ( mGeomData )
    mBounds = mGeomData->getBounds(); // Will automatically recalc if dirty.
  else
    mBounds.setIsDirty ( true );
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


