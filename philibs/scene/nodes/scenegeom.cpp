/////////////////////////////////////////////////////////////////////
//
//    file: scenegeom.cpp
//
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <set>
#include <map>

#include "scenegeom.h"
#include "scenecommon.h"
#include "pnimathstream.h"

/////////////////////////////////////////////////////////////////////

using namespace pni::math;

namespace scene {
  
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void geomData::reallocWithAttributes ( attributes const& newAttributes )
{
  assert ( mAttributes.size() <= newAttributes.size() );
  assert ( ! mAttributes.empty() );
  assert ( ! newAttributes.empty() );

  if ( mAttributes == newAttributes )
    return; // Early exit!!!

  size_t const vertCount = getVertCount();
  size_t const vertLast = vertCount - 1;

  mValues.resize ( newAttributes.getValueStride() * getVertCount() );

  // Walk through src and dst attributes, copying/moving bits sparsely as
  // needed... ideally back to front in-place so we don't have to do
  // two big allocations (but we do have to do the one above).
  
  for ( auto const& attr : mAttributes )
  {
    size_t const srcOffset = mAttributes.getValueOffset(attr.mType);
    size_t const dstOffset = newAttributes.getValueOffset(attr.mType);
    size_t const srcStride = mAttributes.getValueStride();
    size_t const dstStride = newAttributes.getValueStride();
    size_t const attrSize = attr.mComponents * sizeof ( attr.mComponents );
    
    ValueType* src = getValuesPtr() + srcOffset + srcStride * vertLast;
    ValueType* dst = getValuesPtr() + dstOffset + dstStride * vertLast;
    
    for ( size_t num = 0; num < vertCount; ++num, src -= srcStride, dst -= dstStride )
      memcpy(dst, src, attrSize);
  }
  
  attributesOp() = newAttributes;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class genNormalsTri
{
  public:
  
    using Index = geomData::IndexType;
    using Indices = geomData::IndexType[ 3 ];
  
    void set ( Index i0, Index i1, Index i2, geomData const* pData )
      {
        mIndices[ 0 ] = i0; mIndices[ 1 ] = i1; mIndices[ 2 ] = i2;
        
        vec3 p0 ( pData->getAttributePtr(geomData::Position, i0) );
        vec3 p1 ( pData->getAttributePtr(geomData::Position, i1) );
        vec3 p2 ( pData->getAttributePtr(geomData::Position, i2) );

        vec3 v0 = p0 - p1;
        vec3 v1 = p2 - p1;

        mNormal.cross( v1, v0 );
        
          // Calc face area... equal to half of the cross product magnitude
          // as the cross product length results in the area of a parallelogram
          // described by the preceding two edges/vectors.
        mArea = mNormal.length() * 0.5f;

          // normalize normal, of course... later we will multiply it back
          // with area to get weighted normals.
        mNormal.normalize();
      }
  
    Indices mIndices;
    float mArea;
    vec3 mNormal { vec3::NoInit };
};

class genNormalsVert
{
  public:
  
    bool operator < ( genNormalsVert const& rhs ) const
      {
          // This gets us a cheap-ish epsilon lt compare... first test
          // for epsilon equality... if not true then return strict lt,
          // which by definition must now be outside of eps.
          // TODO: Needs to use geomData epsilon instance variable.
        if ( ! mPos.equal(rhs.mPos, mFuzz))
          return mPos < rhs.mPos;
        else
          return false;
      }
  
    vec3 mPos;
    geomData::IndexType mIndex;     // index into geomData values array
    size_t mTri;                    // index into GenNormalsTris list below
    Trait::ValueType mFuzz;
};

  // Can't do this on shared verts because we might have to break shading
  // on edges with an angle greater than the specified angle.
void geomData::generateNormals ( float degrees )
{
  Trait::ValueType const angleRads = Trait::d2r(degrees);
  Trait::ValueType const cosAngle = Trait::cos(angleRads);
  
    // Add normal storage if it's not already present.  Better than making
    // the client do it and having failures if they don't.
  if ( ! mAttributes.hasAttribute(scene::geomData::Normal ) )
  {
    Attributes newAttributes = mAttributes; // deep copy
    newAttributes.push_back ( { CommonAttributeNames[ geomData::Normal], geomData::Normal, geomData::DataType_FLOAT, geomData::NormalComponents } );

    reallocWithAttributes(newAttributes);
  }
  
  size_t const posOffset = mAttributes.getValueOffset(Position);
  
    // Create array of tri structure that includes vert indices, tri normal, and tri area
    // Create array of sharing ids that multimaps vert pos to vert index and tri index
  using GenNormalsTris = std::vector< genNormalsTri >;
  GenNormalsTris tris;
  tris.resize(getTriCount());
  
  using GenNormalsVerts = std::multiset< genNormalsVert >;
  GenNormalsVerts verts;

  size_t numTri = 0;
  triIter titer ( this );
  while (titer)
  {
    IndexType i0 = titer.getCurIndex();
    verts.insert( genNormalsVert { vec3 { &titer + posOffset }, i0, numTri, mFuzz } );
    ++titer;

    IndexType i1 = titer.getCurIndex();
    verts.insert( genNormalsVert { vec3 { &titer + posOffset }, i1, numTri, mFuzz } );
    ++titer;

    IndexType i2 = titer.getCurIndex();
    verts.insert( genNormalsVert { vec3 { &titer + posOffset }, i2, numTri, mFuzz } );
    ++titer;

    tris[ numTri++ ].set ( i0, i1, i2, this );
  }

    // Walk through sharing array to find all tris that share each vert,
    // check if they are within breaking angle, then
    // do weighted avg and assign new normal to all relevant verts
  auto const vertEnd = verts.end();
  for ( auto vertCur = verts.begin(); vertCur != vertEnd; /* nada */ )
  {
    auto upper = verts.upper_bound(*vertCur);
    auto const vertCurOtherStart = vertCur;
    
      // Iterate over set of co-incident verts
    for ( /* nada */ ; vertCur != upper; ++vertCur )
    {
      auto& curTri = tris[ vertCur->mTri ];
      vec3 const curNorm = curTri.mNormal;
      vec3 norm = curNorm * curTri.mArea; // weighted normal

      auto vertCurOther = vertCurOtherStart;
      
        // Walk all connected tris and determine if they
        // should be averaged with this vert's normal
      for ( /* nada */ ; vertCurOther != upper; ++vertCurOther )
      {
          // Skip self as 'norm' is initialized to our normal already
        if ( vertCur == vertCurOther )
          continue;
        
        auto const& otherTri = tris[ vertCurOther->mTri ];
        
          // dot = cos(θ) when normals are unit length
        Trait::ValueType dotVal = curNorm.dot ( otherTri.mNormal );
        if ( dotVal >= cosAngle )
        {
          vec3 const& add = otherTri.mNormal;
          norm += add * otherTri.mArea;   // weighted normal
        }
      }
      
        // normalize blended normal to get the final value.
      norm.normalize();

        // Assign normal in right spot in geomData values array
      norm.copyToArray(getAttributePtr(Normal, vertCur->mIndex));
    }
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void geomData::unshareVerts ()
{
  size_t numTris = getTriCount();

    // Alloc destination geomData
  using DataRef = pni::pstd::autoRef<geomData>;
  DataRef pDst = new geomData;
  pDst->attributesOp() = mAttributes;    // This is a deep copy
  pDst->resizeTrisWithCurrentAttributes(numTris * 3, numTris);
  
  size_t dstStrideBytes = pDst->mAttributes.getValueStrideBytes();

#ifdef DEBUG
  size_t srcStrideBytes = pDst->mAttributes.getValueStrideBytes();
  assert(dstStrideBytes == srcStrideBytes);
#endif // DEBUG
  
  triIter srcTris ( this );
  vertIter dstVerts ( pDst.get() );
  geomData::IndexType indNum = 0;
  
  while ( srcTris )
  {
    void* src = &srcTris;
    void* dst = &dstVerts;

      // Copy values across in bulk
    memcpy(dst, src, dstStrideBytes);

      // New indices just follow the counter
    pDst->getIndices()[ indNum ] = indNum;
  
    ++srcTris;
    ++dstVerts;
    ++indNum;
  }
  
  swap ( pDst.get() );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


class shareVertsVert
{
  public:
  
    shareVertsVert ( geomData::ValueType const* val, geomData::IndexType ind, size_t numVals, Trait::ValueType fuzz ) :
        mVal ( val ), mInd ( ind ), mNumVals ( numVals ), mFuzz ( fuzz ) {}
  
      // Note that this is no good when we support attribute types
      // beyond float.
      // We don't want a binary compare... cause we want an epsilon compare.
    bool operator < ( shareVertsVert const& other ) const
        {
          geomData::ValueType const* rhs = mVal;
          geomData::ValueType const* lhs = other.mVal;
          for ( size_t num = 0; num < mNumVals; ++num )
          {
              // TODO: Use geomData fuzz instance variable
            if ( ! pni::math::Trait::equal(*rhs, *lhs, mFuzz) )
            {
              return *rhs < *lhs;
            }
            else
            {
              ++rhs; ++lhs;
            }
          }
          return false;
        }

    // Public data
  geomData::ValueType const* mVal = 0;
  geomData::IndexType mInd = 0;
  size_t mNumVals = 0;
  Trait::ValueType mFuzz = Trait::fuzzVal;
};

using VertSet = std::multiset<shareVertsVert>;

void geomData::shareVerts ()
{
  size_t const numTris = getTriCount();

    // Build a multiset that collects identical verts in buckets
  VertSet verts;
  triIter srcVerts ( this );
  size_t srcIndex = 0;
  size_t const srcStride = this->mAttributes.getValueStride();
  
  while ( srcVerts )
  {
    verts.insert(shareVertsVert(&srcVerts, srcIndex, srcStride, mFuzz));
    ++srcVerts;
    ++srcIndex;
  }

    // Alloc destination geomData
  using DataRef = pni::pstd::autoRef<geomData>;
  DataRef pDst = new geomData;
  pDst->attributesOp() = mAttributes;    // This is a deep copy
  pDst->resizeTrisWithCurrentAttributes(numTris * 3, numTris);

    // Now walk through the verts, filling in new vert data in dst and setting
    // updated index values in src index slots
  vertIter dstVerts ( pDst.get() );
  size_t dstIndex = 0;
  size_t const dstStrideBytes = pDst->mAttributes.getValueStrideBytes();
  
    // Loop over the whole collection, but note that we don't increment
    // vertCur here.
  auto vertEnd = verts.end();
  for ( auto vertCur = verts.begin(); vertCur != vertEnd; /* nada */ )
  {
      // Copy vertCur src attribute values into dst
    geomData::ValueType* dstVal = &dstVerts;
    
    memcpy(dstVal, vertCur->mVal, dstStrideBytes);

      // Now loop from current to upper_bound to walk through all identical
      // verts... putting the vert in one slot in the values array and all
      // of the necessary slots in the index list.
    auto upper = verts.upper_bound(*vertCur);
    for ( /* nada */; vertCur != upper; ++vertCur )
    {
        // This is importante... we use the original index slot from the
        // src iteration to fill in dst spots as we walk the verts...
        // so the original spots will point to the new shared vert positions.
      pDst->mIndices[ vertCur->mInd ] = dstIndex;
    }
    ++dstVerts;
    ++dstIndex;
  }

    // This makes the logical size right, but STL won't shrink the allocation.
  pDst->getValues().resize(dstIndex * pDst->mAttributes.getValueStride());
  
#ifdef DEBUG
  size_t const srcStrideBytes = this->mAttributes.getValueStrideBytes();
  assert(dstStrideBytes == srcStrideBytes);
#endif // DEBUG

  swap ( pDst.get() );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void geomData::swap ( geomData* rhs )
{
  rhs->mAttributes.swap(mAttributes);
  rhs->mValues.swap(mValues);
  rhs->mIndices.swap(mIndices);
  
  box3 tBounds = rhs->mBounds;
  rhs->mBounds = mBounds;
  mBounds = tBounds;
  
  rhs->setDirty();
  setDirty();
}

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
  size_t incr = mAttributes.getValueStride ();
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

void geom::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  node::collectRefs ( refs );
  refs.push_back ( mGeomData.get () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace scene 


