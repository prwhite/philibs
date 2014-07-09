/////////////////////////////////////////////////////////////////////
//
//    file: sceneisectdd.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneisectdd.h"

// #include "scenecamera.h"
#include "scenegeom.h"
#include "scenegeomfx.h"
#include "scenegroup.h"
#include "scenelight.h"

/////////////////////////////////////////////////////////////////////

using namespace pni::math;

namespace scene {
  
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void isectSimplePartition::fillRoots ()
{
  mRootPtrs.clear ();
  mRootPtrs.resize ( mPartitions.size () );
  
  for ( size_t num = 0; num < mPartitions.size (); ++num )
    mRootPtrs[ num ] = num;
}

/////////////////////////////////////////////////////////////////////

size_t isectSimplePartition::allocPart ()
{
  mPartitions.resize ( mPartitions.size () + 1 );
  return mPartitions.size () - 1;
}

/////////////////////////////////////////////////////////////////////

void isectSimplePartition::optimize ( size_t count )
{
    // Make sure there are some valid roots to start with.
  if ( mRootPtrs.empty () )
    fillRoots ();

//printf ( "starting with %d roots\n", mRootPtrs.size () );

    // Figure out how many children to have in each root.
  size_t newCount = count ? count : mRootPtrs.size ();            
  
  if ( count == 0 )
    newCount = ( size_t ) Trait::sqrt ( ( float ) newCount );
  
    // Setup new root pointers and counter to figure out
    // when to allocate a new partition.
  size_t partCounter = 1; // Yes... 1.
  
  PartPtrs newRootPtrs;
  newRootPtrs.reserve ( newCount + 1 );
  newRootPtrs.push_back ( allocPart () );
  
  for ( size_t curRootPtr = 0; curRootPtr < mRootPtrs.size (); ++curRootPtr )
  {
    mPartitions[ newRootPtrs.back () ].mParts.push_back ( 
        mRootPtrs[ curRootPtr ] );
    
    mPartitions[ newRootPtrs.back () ].mBounds.extendBy (
        mPartitions[ mRootPtrs[ curRootPtr ] ].mBounds );
    
    ++partCounter;
    
    if ( partCounter == newCount )
    {
      partCounter = 1;
      newRootPtrs.push_back ( allocPart () );
    }
  }
  
  mRootPtrs = newRootPtrs;

//printf ( "finishing with %d roots\n", mRootPtrs.size () );
}    

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

isectDd::isectDd() :
  mRecursionLevel ( 0 )
{
  
}

isectDd::~isectDd()
{
  
}

// isectDd::isectDd(isectDd const& rhs)
// {
//   
// }
// 
// isectDd& isectDd::operator=(isectDd const& rhs)
// {
//   
//   return *this;
// }
// 
// bool isectDd::operator==(isectDd const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from graphDd

void isectDd::startGraph ( node const* pNode )
{
  if ( ! mTest )
    return;
    
  if ( mTest->mSegs.empty () )
    return;
  
  mSegsStack.push ( mTest->mSegs );
  
//   mTest->initHits ();
  
   mRecursionLevel = 0;
  
   pNode->accept ( this );
  
  for ( size_t num = 0; num < mTest->mHits.size (); ++num )
  {
    mTest->mHits[ num ].mNodePath.reverse ();
  }
  
  // mTest->mSegs.clear ();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void isectDd::pushAndXformSegs ( pni::math::matrix4 const& mat )
{
  mSegsStack.push ( mSegsStack.top () );
  
  Segs& top = mSegsStack.top ();
  
  Segs::iterator end = top.end ();
  for ( Segs::iterator cur = top.begin ();
      cur != end;
      ++cur )
  {
    cur->xform4 ( *cur, mat );
  }
}

void isectDd::pushSegs ()
{
  mSegsStack.push ( mSegsStack.top () );  
}

void isectDd::popSegs ()
{
  mSegsStack.pop ();
}

bool isectDd::cullSegs ( node const* pNode )
{
  box3 const& box ( pNode->getBounds () );
  
  bool active = false;
  
  Segs& segs = mSegsStack.top ();
  Segs::iterator end = segs.end ();
  for ( Segs::iterator cur = segs.begin ();
      cur != end;
      ++cur )
  {
    if ( box.contains ( *cur ) )
    {
      cur->mActive = true;
      active = true;
    }
    else
    {
      cur->mActive = false;
    }
  }
  
  return active;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// TRICKY Sometimes we don't want to transform the seg set by the
// first node's transform because the segs are already in that
// node's coordinate frame.

bool isectDd::ignoreFirstXform () const
{
  return ( ( mTest->getOps () & isectTest::IgnoreFirstXform  ) &&
    ( mRecursionLevel == 0 ) );
}

bool isectDd::dispatchPre ( node const* pNode )
{
  if ( ! ( mTravMask & pNode->getTravMask ( Isect ) ) )
    return false;
  
  mNodeStack.push ( const_cast< node* > ( pNode ) );
  
  if ( ! pNode->getMatrix ().getIsIdent() )
  {
//     mMatStack.push ();
//     mMatStack->preMult ( pNode->getMatrix () );
    
    if ( ! ignoreFirstXform () )
    {
      matrix4 tmp ( pNode->getMatrix () );
      tmp.invert ();
      pushAndXformSegs ( tmp );
    }
    else
      pushSegs ();
  }
  else
    pushSegs ();

  ++mRecursionLevel;

  if ( cullSegs ( pNode ) )
  {
    return true;
  }
  else
  {
    dispatchPost ( pNode );
    return false;
  }
}

/////////////////////////////////////////////////////////////////////

void isectDd::dispatchPost ( node const* pNode )
{
  mNodeStack.pop ();
  
  --mRecursionLevel;

//   if ( ! pNode->getMatrix ().getIsIdent () )
//     mMatStack.pop ();

  popSegs ();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void isectDd::dispatch ( camera const* pNode )
{
  // NO-OP
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// From http://jgt.akpeters.com/papers/MollerTrumbore97/ .
// 
// This has some interesting comments about this algorithm and
// an alternative implementation:
// http://geometryalgorithms.com/Archive/algorithm_0105/algorithm_0105.htm#Segment-Triangle
//

// #define EPSILON 0.000001
/*	#define CROSS(dest,v1,v2) \
          	dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          	dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          	dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
	#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
	#define SUB(dest,v1,v2)
          	dest[0]=v1[0]-v2[0]; \
          	dest[1]=v1[1]-v2[1]; \
          	dest[2]=v1[2]-v2[2]; */
// 
// int
// intersect_triangle(double orig[3], double dir[3],
//                    double vert0[3], double vert1[3], double vert2[3],
//                    double *t, double *u, double *v)
// {
//    double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
//    double det,inv_det;
// 
//    /* find vectors for two edges sharing vert0 */
//    SUB(edge1, vert1, vert0);
//    SUB(edge2, vert2, vert0);
// 
//    /* begin calculating determinant - also used to calculate U parameter */
//    CROSS(pvec, dir, edge2);
// 
//    /* if determinant is near zero, ray lies in plane of triangle */
//    det = DOT(edge1, pvec);
// 
// #ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
//    if (det < EPSILON)
//       return 0;
// 
//    /* calculate distance from vert0 to ray origin */
//    SUB(tvec, orig, vert0);
// 
//    /* calculate U parameter and test bounds */
//    *u = DOT(tvec, pvec);
//    if (*u < 0.0 || *u > det)
//       return 0;
// 
//    /* prepare to test V parameter */
//    CROSS(qvec, tvec, edge1);
// 
//     /* calculate V parameter and test bounds */
//    *v = DOT(dir, qvec);
//    if (*v < 0.0 || *u + *v > det)
//       return 0;
// 
//    /* calculate t, scale parameters, ray intersects triangle */
//    *t = DOT(edge2, qvec);
//    inv_det = 1.0 / det;
//    *t *= inv_det;
//    *u *= inv_det;
//    *v *= inv_det;
// #else                    /* the non-culling branch */
//    if (det > -EPSILON && det < EPSILON)
//      return 0;
//    inv_det = 1.0 / det;
// 
//    /* calculate distance from vert0 to ray origin */
//    SUB(tvec, orig, vert0);
// 
//    /* calculate U parameter and test bounds */
//    *u = DOT(tvec, pvec) * inv_det;
//    if (*u < 0.0 || *u > 1.0)
//      return 0;
// 
//    /* prepare to test V parameter */
//    CROSS(qvec, tvec, edge1);
// 
//    /* calculate V parameter and test bounds */
//    *v = DOT(dir, qvec) * inv_det;
//    if (*v < 0.0 || *u + *v > 1.0)
//      return 0;
// 
//    /* calculate t, ray intersects triangle */
//    *t = DOT(edge2, qvec) * inv_det;
// #endif
//    return 1;
// }


void isectDd::isectTri ( 
  pni::math::vec3 const& vert0,
  pni::math::vec3 const& vert1,
  pni::math::vec3 const& vert2,
  seg const& tseg, hit& ahit,
  unsigned short startInd )
{
  using namespace pni;
  typedef float ValueType;

	math::vec3 const& dir = tseg.getDir ();
	math::vec3 const& orig = tseg.getPos ();
	float const len = tseg.getLength ();

	/* find vectors for two edges sharing vert0 */
	math::vec3 edge1 ( vert1 );
	edge1 -= vert0;
	//edge1 *= 1.0f + pni::math::Trait::fuzzVal;  // Experimental
	
	math::vec3 edge2 ( vert2 );
	edge2 -= vert0;
	//edge2 *= 1.0f + pni::math::Trait::fuzzVal;  // Experimental

// std::cout << "  edge 1, 2: \n    "
// 		<< edge1 << "\n    "
// 		<< edge2 << std::endl;

	/* begin calculating determinant - also used to calculate U parameter */
	math::vec3 pvec ( math::vec3::NoInit );
	pvec.cross ( dir, edge2 );

	/* if determinant is near zero, ray lies in plane of triangle */
	ValueType det = edge1.dot ( pvec );

	///// Do back/frontface cull tests.
	bool isBack = det < math::Trait::fuzzVal;
	if ( /* CullBack && */ isBack )
  	return;

// std::cout << "  det = " << det << std::endl;
// std::cout << "  passed back face cull " << CullBack << std::endl;
  
	bool isFront = det > -math::Trait::fuzzVal;
//   if ( CullFront && isFront )
//   	return false;

// std::cout << "  passed front face cull " << CullFront << std::endl;

	// Catch when ray is parallel to the plane.
// 	if ( ! CullFront && ! CullBack )
// 	{
		if ( ! isFront && ! isBack )
			return;
// 	}


// std::cout << "  passed parallel cull " << std::endl;

	// Handle back facing math without additional conditionals.
	// TODO bench this vs conditionals and other approaches.
// 	const ValueType mult = isFront ? 1.0f : -1.0f;
// 	det *= mult;

	/* calculate distance from vert0 to ray origin */
	math::vec3 tvec ( orig );
	tvec -= vert0;

	/* calculate U parameter and test bounds */
// 	ValueType u = mult * tvec.dot ( pvec );
	ValueType u = tvec.dot ( pvec );

// std::cout << "  u val = " << u << std::endl;

	if ( u < math::Trait::zeroVal || u > det )
  	return;

// std::cout << "  passed u bounds test" << std::endl;

	/* prepare to test V parameter */
	math::vec3 qvec ( math::vec3::NoInit );
	qvec.cross ( tvec, edge1 );

	// TODO Possibly cull based on t > seg.t here.

	/* calculate V parameter and test bounds */
// 	ValueType v = mult * dir.dot ( qvec );
	ValueType v = dir.dot ( qvec );
// std::cout << "  v val = " << v << std::endl;

	if ( v < math::Trait::zeroVal || u + v > det )
  	return;

// std::cout << "  passed v bounds test" << std::endl;

	/* calculate t, scale parameters, ray intersects triangle */
// 	ValueType t = mult * edge2.dot ( qvec );
	ValueType t = edge2.dot ( qvec );
	ValueType inv_det = math::Trait::oneVal / det;
	t *= inv_det;
	u *= inv_det;
	v *= inv_det;

	/////// Clip result to seg length.
	// Should we do an fabs here to support - lengths?
	// No... cause that would get flipped in the seg.xform.
	if ( t <= len && t >= math::Trait::zeroVal )
	{
// std::cout << "  passed ray length test" << std::endl;

		// Rescale t to be in [0,1] range.  This makes t coordinate
		// system (esp. scaling) independent.
		t /= tseg.getLength ();

		if ( t < ahit.mTval )
		{
			ahit.mTval = t;
			ahit.mBarycentricUV.set ( u, v );
// 			ahit.mpNode = mNodeStack.back ();
			ahit.mNodePath = mNodeStack;
			ahit.mStartIndex = startInd;
// 			ahit.mPrimIndex = mPrimIndex;
// 			ahit.mPrimNum = mPrimNum;
// 			request.mHits.insert ( ahit );

      if ( mTest->getOps () & isectTest::CalcNormal )
        ahit.mNormal.cross ( edge1, edge2  );
		}
	}
}

/////////////////////////////////////////////////////////////////////

void isectDd::isectGeomData ( geom const* pGeom, seg const& aSeg, hit& aHit )
{
  geomData const* pGdata = pGeom->geomDataProp().get();
  triIter iter ( const_cast< geomData* > ( pGdata ) );
  size_t posOffset = pGdata->mBinding.getValueOffsetBytes( geomData::Position );
  size_t startInd = 0;
  
  while ( iter )
  {
    vec3 const& vert0 ( iter.get< vec3 >( posOffset ) );
    ++iter;
    vec3 const& vert1 ( iter.get< vec3 >( posOffset ) );
    ++iter;
    vec3 const& vert2 ( iter.get< vec3 >( posOffset ) );
    ++iter;
    
    isectTri ( vert0, vert1, vert2, aSeg, aHit, startInd );
    
    startInd += 3;
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void isectDd::isectTravData ( geom const* pGeom, seg const& aSeg, hit& aHit )
{
  isectSimplePartition* pData = 
      reinterpret_cast< isectSimplePartition* > ( pGeom->getTravData ( Isect ) );

  if ( pData->mRootPtrs.empty () )
    isecTravDataFlat ( pGeom, aSeg, aHit );
  else
    isectTravDataTree ( pGeom, aSeg, aHit );
}

/////////////////////////////////////////////////////////////////////

void isectDd::isecTravDataFlat ( geom const* pGeom, seg const& aSeg, hit& aHit )
{
  isectSimplePartition* pData =
      reinterpret_cast< isectSimplePartition* > ( pGeom->getTravData ( Isect ) );
    
  typedef isectSimplePartition::Partitions::iterator PartIter;
  PartIter partEnd = pData->mPartitions.end ();
  geomData const* pGdata = pGeom->geomDataProp().get ();
  size_t posOffset = pGdata->mBinding.getValueOffsetBytes( geomData::Position );
  
  for ( PartIter partCur = pData->mPartitions.begin (); partCur != partEnd; ++partCur )
  {
    float tNear = 0.0f;
    float tFar = 0.0f;
    
    triIter tIter ( const_cast< geom* > ( pGeom ) );

    if ( partCur->mBounds.isect ( aSeg, tNear, tFar ) )
    {
      typedef isectSimplePartition::Indices::const_iterator IndIter;
      IndIter indEnd = partCur->mIndices.end ();
      for ( IndIter indCur = partCur->mIndices.begin (); indCur != indEnd; )
      {
        size_t startInd = *indCur;
        
        vec3 const& vert0 ( tIter.get<vec3>(*indCur, posOffset) );
        ++indCur;
        vec3 const& vert1 ( tIter.get<vec3>(*indCur, posOffset) );
        ++indCur;
        vec3 const& vert2 ( tIter.get<vec3>(*indCur, posOffset) );
        ++indCur;
        
        isectTri ( vert0, vert1, vert2, aSeg, aHit, startInd );
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////

  // Starts the recursion of the partition tree, beginning with the
  // roots.
void isectDd::isectTravDataTree ( geom const* pGeom, seg const& aSeg, hit& aHit )
{
//printf ( "isect trav data tree start\n" );
  isectSimplePartition* pData = 
      reinterpret_cast< isectSimplePartition* > ( pGeom->getTravData ( Isect ) );

    // Walk through each root... get the part object and invoke the
    // tree recursion with that part.
  size_t end = pData->mRootPtrs.size ();
  for ( size_t cur = 0; cur != end; ++cur )
  {
    isectSimplePartition::Partition& part = pData->mPartitions[
        pData->mRootPtrs[ cur ] ];
    
    isectTravDataTreePartition ( pGeom, part, aSeg, aHit );
  }
}

/////////////////////////////////////////////////////////////////////

void isectDd::isectTravDataTreePartition ( geom const* pGeom, 
    isectSimplePartition::Partition const& part, seg const& aSeg, hit& aHit )
{
//printf ( "isect trav data tree part\n" );
  float tNear = 0.0f;
  float tFar = 0.0f;
    
  if ( part.mBounds.isect ( aSeg, tNear, tFar ) )
  {
    if ( part.mParts.empty () )
    {
        // No parts, must be a leaf (by definition).
#define INLINEISECT
#ifdef INLINEISECT
        // COPY/PASTE: Code inline'd from isectTravDataTreeLeaf.
      triIter tIter ( const_cast< geom* > ( pGeom ) );
      size_t posOffset = pGeom->geomDataProp()->mBinding.getValueOffsetBytes(geomData::Position);

      typedef isectSimplePartition::Indices::const_iterator IndIter;
      IndIter indEnd = part.mIndices.end ();
      for ( IndIter indCur = part.mIndices.begin (); indCur != indEnd; )
      {
        size_t startInd = *indCur;
        
        vec3 const& vert0 ( tIter.get<vec3>(*indCur, posOffset) );
        ++indCur;
        vec3 const& vert1 ( tIter.get<vec3>(*indCur, posOffset) );
        ++indCur;
        vec3 const& vert2 ( tIter.get<vec3>(*indCur, posOffset) );
        ++indCur;
        
        isectTri ( vert0, vert1, vert2, aSeg, aHit, startInd );
      }
#else
      isectTravDataTreeLeaf ( pGeom, part, aSeg, aHit );
#endif
    }
    else
    {
        // Valid parts, must be an internal node (by definition).
      isectSimplePartition* pData = 
          reinterpret_cast< isectSimplePartition* > ( pGeom->getTravData ( Isect ) );

      size_t end = part.mParts.size ();
      for ( size_t cur = 0; cur != end; ++cur )
      {
        isectSimplePartition::Partition& childPart = pData->mPartitions[
            part.mParts[ cur ] ];

          // Recursion.
          // TODO: Consider unwinding this... it never really gets
          // that deep, but it's a lot of function invocations.
        isectTravDataTreePartition ( pGeom, childPart, aSeg, aHit );
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////

inline  // No chance :)
void isectDd::isectTravDataTreeLeaf ( geom const* pGeom, 
    isectSimplePartition::Partition const& part, seg const& aSeg, hit& aHit )
{
  triIter tIter ( const_cast< geom* > ( pGeom ) );
  size_t posOffset = pGeom->geomDataProp()->mBinding.getValueOffsetBytes(geomData::Position);

  typedef isectSimplePartition::Indices::const_iterator IndIter;
  IndIter indEnd = part.mIndices.end ();
  for ( IndIter indCur = part.mIndices.begin (); indCur != indEnd; )
  {
    size_t startInd = *indCur;
    
    vec3 const& vert0 ( tIter.get<vec3>(*indCur, posOffset) );
    ++indCur;
    vec3 const& vert1 ( tIter.get<vec3>(*indCur, posOffset) );
    ++indCur;
    vec3 const& vert2 ( tIter.get<vec3>(*indCur, posOffset) );
    ++indCur;
    
    isectTri ( vert0, vert1, vert2, aSeg, aHit, startInd );
  }
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


void isectDd::isectGeom ( geom const* pGeom, seg const& aSeg, hit& aHit )
{
  if ( mTest->getOps () & isectTest::OnlyGeomBounds )
  {
    // What happens when the seg is completely inside the box?
    // Check box.isect code to see how this is handled someday soon.
    // Update: a ray completely enclosed in a box will return hits
    // with a tvalue < 0.  This needs to be caught somewhere if
    // we intend to treat boxes as solids rather than shells.
  
    float tnear, tfar;  // Uninitialized, initialized in isect call.
    pGeom->getBounds ().isect ( aSeg, tnear, tfar );
    tnear /= aSeg.getLength (); // Get in [0,1].
    
    if ( tnear < aHit.mTval )
    {
      aHit.mTval = tnear;
      aHit.mNodePath = mNodeStack;
//       aHit.mNodePath.reverse ();
      aHit.mStartIndex = 0;
    }
  }
  else
  {
    if ( pGeom->getTravData ( Isect ) )
      isectTravData ( pGeom, aSeg, aHit ); // Partitioned geom indices.
    else
      isectGeomData ( pGeom, aSeg, aHit );
  }
}

/////////////////////////////////////////////////////////////////////

void isectDd::dispatch ( geom const* pNode )
{
  if ( dispatchPre ( pNode ) )
  {
    Segs const& segs = mSegsStack.top ();
    
    for ( size_t num = 0; num < segs.size (); ++num )
    {
      seg const& aseg = segs[ num ];
      
      if ( aseg )  // Calling segs::operator bool.  :>
      {
        hit& ahit = mTest->mHits[ num ];
        isectGeom ( pNode, aseg, ahit );
      }
    }
        
    dispatchPost ( pNode );
  }
}

/////////////////////////////////////////////////////////////////////

void isectDd::dispatch ( geomFx const* pNode )
{
  geom const* pGeom = pNode;
  dispatch ( pGeom );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void isectDd::dispatch ( group const* pNode )
{
  if ( dispatchPre ( pNode ) )
  {
    node::Nodes const& kids = pNode->getChildren ();
    
    node::Nodes::const_iterator end = kids.end ();
    for ( node::Nodes::const_iterator cur = kids.begin ();
        cur != end;
        ++cur )
    {
      ( *cur )->accept ( this );
    }
    
    dispatchPost ( pNode );
  }
}

/////////////////////////////////////////////////////////////////////

void isectDd::dispatch ( light const* pNode )
{
  // NO-OP
}

/////////////////////////////////////////////////////////////////////
void isectDd::dispatch ( sndEffect const* pNode )
{
  // NO-OP
}

/////////////////////////////////////////////////////////////////////

void isectDd::dispatch ( sndListener const* pNode )
{
  // NO-OP
}

/////////////////////////////////////////////////////////////////////



} // partEnd of namespace scene 


