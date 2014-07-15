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

void geomData::unshareVerts ()
{
  size_t numTris = getTriCount();

    // Alloc destination geomData
  geomData* pDst = new geomData;
  pDst->mBinding = mBinding;    // This is a deep copy
  pDst->resizeTrisWithCurrentBinding(numTris * 3, numTris);
  
  size_t dstStrideBytes = pDst->mBinding.getValueStrideBytes();

#ifdef DEBUG
  size_t srcStrideBytes = pDst->mBinding.getValueStrideBytes();
  assert(dstStrideBytes == srcStrideBytes);
#endif // DEBUG
  
  triIter srcTris ( this );
  vertIter dstVerts ( pDst );
  geomData::IndexType indNum = 0;
  
  while ( srcTris )
  {
    void* src = &srcTris.get<void*>(0);
    void* dst = &dstVerts.get<void*>(0);

      // Copy values across in bulk
    memcpy(dst, src, dstStrideBytes);

      // New indices just follow the counter
    pDst->getIndices()[ indNum ] = indNum;
  
    ++srcTris;
    ++dstVerts;
    ++indNum;
  }
  
  swap ( *pDst );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class shareVertsVert
{
  public:
    using ValueType = geomData::ValueType;
  
    shareVertsVert ( geomData const* pData, ValueType const* val, geomData::IndexType ind ) :
        mData ( pData ), mVal ( val ), mInd ( ind ) {}
  
    bool operator < ( shareVertsVert const& other ) const
        {
          ValueType const* rhs = mVal;
          ValueType const* lhs = other.mVal;
          
          return mData->lt(lhs, rhs);
        }

    // Public data
  geomData const* mData = 0;
  ValueType const* mVal = 0;
  ValueType mInd = 0;
};

void geomData::shareVerts ()
{
  size_t const numTris = getTriCount();

    // Build a multiset that collects identical verts in buckets
  using VertSet = std::multiset<shareVertsVert>;
  VertSet verts;
  triIter srcVerts ( this );
  size_t srcIndex = 0;

  while ( srcVerts )
  {
    verts.insert(shareVertsVert(this, &srcVerts.get< uint8_t >( 0 ), srcIndex));
    ++srcVerts;
    ++srcIndex;
  }

    // Alloc destination geomData
  geomData* pDst = new geomData;
  pDst->mBinding = mBinding;    // This is a deep copy
  pDst->resizeTrisWithCurrentBinding(numTris * 3, numTris);

    // Now walk through the verts, filling in new vert data in dst and setting
    // updated index values in src index slots
  vertIter dstVerts ( pDst );
  size_t dstIndex = 0;
  size_t const dstStrideBytes = pDst->mBinding.getValueStrideBytes();
  
    // Loop over the whole collection, but note that we don't increment
    // vertCur here.
  auto vertEnd = verts.end();
  for ( auto vertCur = verts.begin(); vertCur != vertEnd; /* nada */ )
  {
      // Copy vertCur src attribute values into dst
    geomData::ValueType* dstVal = &dstVerts.get<geomData::ValueType>(0);

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
      pDst->getIndices()[ vertCur->mInd ] = dstIndex;
    }
    ++dstVerts;
    ++dstIndex;
  }

    // This makes the logical size right, but STL won't shrink the allocation.
  pDst->resize(dstIndex, pDst->getIndices().size());
  
#ifdef DEBUG
  size_t const srcStrideBytes = this->mBinding.getValueStrideBytes();
  assert(dstStrideBytes == srcStrideBytes);
#endif // DEBUG

  swap ( *pDst );
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void geomData::dbg ( std::ostream& ostr ) const
{
  using namespace std;

  ostr << "geomData : " << this << endl;

  size_t stride = mBinding.getValueStrideBytes();
  size_t numVerts = getIndices().size();
  ValueType const* pVals = getPtr< ValueType >();

  ostr << " attributes: num verts = " << numVerts <<
      " num bytes= " << sizeBytes() <<
      " stride = " << stride << std::endl;
  ostr << " indices: num indices = " << getIndices().size () << endl;

  for ( size_t num = 0; num < numVerts; ++num )
  {
    ostr << num << " ";
    for ( auto binding : mBinding )
    {
      ostr << binding.mName << " = ";

      this->ostream ( ostr, binding.mCount, binding.mDataType, pVals );
      ostr << "; ";
      
      size_t const incr = binding.mCount * binding.mSize;
      pVals += incr;
    }
    ostr << endl;
  }

  auto end = getIndices().size();
  for ( size_t num = 0; num < end; ++num )
    ostr << "  " << num << " " << getIndices()[ num ] << endl;
}

void geomData::updateBounds () const
{
  geomData* pncData = const_cast< geomData* >(this);

  if ( boundsProp().getDirty() )
  {
    box3& bounds = pncData->boundsProp().op(); // DirtyBounds::mVal;

    bounds.setEmpty ();
    
    if ( getIndices().size () == 0 )
      return;

    auto pend = end< vec3 > ( Position );
    for ( auto pcur = begin< vec3 > ( Position ); pcur < pend; ++pcur )
      bounds.extendBy ( *pcur );
    
    pncData->boundsProp().setDirty( false );
    bounds.setIsDirty(false);
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void geom::setGeomBoundsDirty ()
{
  node::setBoundsDirty ();
  if ( mGeomData )
    mGeomData->boundsProp().setDirty();
}

/////////////////////////////////////////////////////////////////////
// First entry point when bounds are being cleaned up,
// introduced by scene::node a la template method pattern.

void geom::updateBounds () const
{
  if ( mBoundsMode == ForceEmpty )
  {
    if ( ! mBounds.isEmpty () )
    {
      mBounds.setEmpty ();  // needs to set bounds dirty too?
    }      
  }
  else
  {
    if ( ( ( mBoundsMode & Static ) && mBounds.isEmpty () )
        || ( mBoundsMode & Dynamic ) )
    {
      generateGeomBounds ();
      
//std::cout << "geom bounds for " << getName() << " " << mBounds << std::endl;
      
      if ( mBoundsMode & Partition )
        generateGeomPartition ();
    }
  }

  mBounds.setIsDirty ( false );
}

/////////////////////////////////////////////////////////////////////

void geom::generateGeomBounds () const
{
  if ( mGeomData )
    mBounds = mGeomData->boundsProp().getUpdated();
  else
    mBounds.setIsDirty ( true );
}

void geom::generateGeomPartition () const
{
  // TODO
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class genNormalsTri
{
  public:
  
    using Index = geomData::IndexType;
    using Indices = geomData::IndexType[ 3 ];
    using Iter = geomData::const_iterator<vec3>;
      
    void set ( Index i0, Index i1, Index i2, Iter const& posBeg )
      {
        mIndices[ 0 ] = i0; mIndices[ 1 ] = i1; mIndices[ 2 ] = i2;

        auto piter0 = posBeg + i0;
        auto piter1 = posBeg + i1;
        auto piter2 = posBeg + i2;
        
        vec3 p0 ( *piter0 );
        vec3 p1 ( *piter1 );
        vec3 p2 ( *piter2 );
        
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
    geomData::IndexType mIndex;      // index into geomData values array
    size_t mTri;                        // index into GenNormalsTris list below
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
  if ( ! mBinding.hasBinding ( Normal ) )
  {
    Binding newBindings = mBinding;
    
//  pLineData->mBinding.push_back ( { {}, lineData::Position, lineData::Float, sizeof(float), 3 } );
    
    newBindings.push_back( { CommonAttributeNames[ geomData::Normal ], Normal,
        DataType_FLOAT, sizeof ( float ), NormalComponents } );
    
    migrate(newBindings);
  }

  size_t const posOffset = mBinding.getValueOffsetBytes(Position);
  
    // Create array of tri structure that includes vert indices, tri normal, and tri area
    // Create array of sharing ids that multimaps vert pos to vert index and tri index
  using GenNormalsTris = std::vector< genNormalsTri >;
  GenNormalsTris tris;
  tris.resize(getTriCount());
  
  using GenNormalsVerts = std::multiset< genNormalsVert >;
  GenNormalsVerts verts;

  size_t numTri = 0;
  triIter titer ( this );
  genNormalsTri::Iter posBeg = begin<vec3>(Position);
  
  Trait::ValueType const eps = getEpsilon< Trait::ValueType >();
  
  while (titer)
  {
    IndexType i0 = titer.getCurIndex();
    verts.insert( genNormalsVert { titer.get< vec3 >(posOffset), i0, numTri, eps } );
//    verts.insert( genNormalsVert { vec3 { &titer + posOffset }, i0, numTri, mFuzz } );
    ++titer;

    IndexType i1 = titer.getCurIndex();
    verts.insert( genNormalsVert { titer.get< vec3 >(posOffset), i1, numTri, eps } );
    ++titer;

    IndexType i2 = titer.getCurIndex();
    verts.insert( genNormalsVert { titer.get< vec3 >(posOffset), i2, numTri, eps } );
    ++titer;

    tris[ numTri++ ].set ( i0, i1, i2, posBeg );
  }

    // Walk through sharing array to find all tris that share each vert,
    // check if they are within breaking angle, then
    // do weighted avg and assign new normal to all relevant verts
  auto normBeg = begin< vec3 > ( Normal );
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
      auto normCur = normBeg + vertCur->mIndex;
      *normCur = norm;
    }
  }
}


} // end of namespace scene 


