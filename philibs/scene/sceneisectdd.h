/////////////////////////////////////////////////////////////////////
//
//    class: isectDd
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneisectdd_h
#define scenesceneisectdd_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include "scenegraphdd.h"
#include "scenenodepath.h"
#include "scenegeom.h"

#include "pnirefcount.h"
#include "pnivec2.h"
#include "pniseg.h"
#include "pnimatrix4.h"
#include "pnimatstack.h"

#include <vector>
#include <stack>

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

  // This class conceptually is an arbitrary, unordered, non-hierarchical
  // collection of tri indices coupled to their collective bounding box.
  
  // It is optionally used to get a moderate speed up for collision detection
  // and will be stored via a nodes travData with the Isect selector.

class isectSimplePartition :
  public attr
{
  public:
    virtual attr* dup () const { return 0; }
  
  
    typedef geomData::Indices Indices;
    typedef size_t PartPtr;
    typedef std::vector< PartPtr > PartPtrs;
    
    struct Partition
    {
      pni::math::box3 mBounds;
      Indices mIndices;
      PartPtrs mParts;
    };
    
    typedef std::vector< Partition > Partitions;
    
    Partitions mPartitions;
    PartPtrs mRootPtrs;
    
    void optimize ( size_t count );
  
  protected:
    void fillRoots ();
    size_t allocPart ();
};

/////////////////////////////////////////////////////////////////////

class isectTest :
  public pni::pstd::refCount
{
  protected:
    virtual ~isectTest () {}
    void collectRefs ( pni::pstd::refCount::Refs& refs ) 
        {
          Hits::iterator end = mHits.end ();
          for ( Hits::iterator cur = mHits.begin (); cur != end; ++cur )
            cur->mNodePath.collectRefs ( refs );
        }

  public:
  
    enum Ops {
      Default = 0x00,
      OnlyGeomBounds = 0x01,
      IgnoreFirstXform = 0x02,
      CalcNormal = 0x04
    };
  
    struct hit
    {
      hit () :
        mTval ( pni::math::Trait::maxVal )
          {
          }
    
      nodePath mNodePath;
      float mTval;
      pni::math::vec2 mBarycentricUV;
      unsigned int mStartIndex;
      pni::math::vec3 mNormal;      // Not normalized!!!
      
      bool isValid () { return mTval < pni::math::Trait::maxVal; }
    };
    
    class seg :
      public pni::math::seg
    {
      public:
        seg () : mActive ( true ) {}
        seg ( pni::math::seg const& orig ) :
          pni::math::seg ( orig ),
          mActive ( true ) {}

        operator bool () const { return mActive; }

        bool mActive;
    };
    
    isectTest () :
      mOps ( Default )
        {
        }
        
    void xform ( pni::math::matrix4 const& mat4 )
        {
          Segs::iterator end = mSegs.end ();
          for ( Segs::iterator cur = mSegs.begin ();
              cur != end;
              ++cur )
          {
            cur->xform4 ( *cur, mat4 );
          }
        }
  
    void setOps ( unsigned int ops ) { mOps = ops; }
    unsigned int getOps () const { return mOps; }
  
    void initSegs ( int howMany ) { mSegs.clear (); mSegs.resize ( howMany ); initHits (); }
    void clearHits () { mHits.clear (); }
    void initHits () { clearHits (); mHits.resize ( mSegs.size () ); }
  
    // Types.
    typedef std::vector< seg > Segs;
    typedef std::vector< hit > Hits;
    
    // Data members.
    // Elements in mHits directly correspond to elements in mSegs, 
    // i.e., mSegs[ 0 ] will have hit result in mHits[ 0 ], etc.
    Segs mSegs;
    Hits mHits;
    unsigned int mOps;
      
};

/////////////////////////////////////////////////////////////////////

class isectDd :
  public graphDd
{
  public:
    isectDd();
    virtual ~isectDd();
//     isectDd(isectDd const& rhs);
//     isectDd& operator=(isectDd const& rhs);
//     bool operator==(isectDd const& rhs) const;
    
    typedef pni::pstd::autoRef< isectTest > TestRef;
    typedef isectTest::seg seg;
    typedef isectTest::Segs Segs;
    typedef isectTest::hit hit;
    typedef isectTest::Hits Hits;
    
    void setTest ( isectTest* pTest ) { mTest = pTest; }
    isectTest* getTest () const { return mTest.get (); }
    
  protected:

    bool dispatchPre ( node const* pNode );
    void dispatchPost ( node const* pNode );
    
    bool ignoreFirstXform ();
    void pushAndXformSegs ( pni::math::matrix4 const& mat );
    void pushSegs ();
    void popSegs ();
    bool cullSegs ( node const* pNode );
    void isectGeom ( geom const* pGeom, seg const& aSeg, hit& aHit );
    void isectGeomData ( geom const* pGeom, seg const& aSeg, hit& aHit );
    void isectTravData ( geom const* pGeom, seg const& aSeg, hit& aHit );
    void isecTravDataFlat ( geom const* pGeom, seg const& aSeg, hit& aHit );
    void isectTravDataTree ( geom const* pGeom, seg const& aSeg, hit& aHit );
    void isectTravDataTreePartition ( geom const* pGeom, 
        isectSimplePartition::Partition const& part, seg const& aSeg, hit& aHit );
    void isectTravDataTreeLeaf ( geom const* pGeom, 
        isectSimplePartition::Partition const& part, seg const& aSeg, hit& aHit );
    void isectTri ( 
        pni::math::vec3 const& vert0,
        pni::math::vec3 const& vert1,
        pni::math::vec3 const& vert2,
        seg const& aSeg, hit& aHit,
        unsigned short startInd );

  private:
    typedef std::stack< isectTest::Segs > SegsStack;
    typedef pni::math::matStack< pni::math::matrix4 > MatStack4;
    typedef nodePath NodeStack;

    TestRef mTest;
//     MatStack4 mMatStack;
    SegsStack mSegsStack;
    NodeStack mNodeStack;
    unsigned int mRecursionLevel;

  // interface from graphDd
  public:

    virtual void startGraph ( node const* pNode );

  protected:
    virtual void dispatch ( camera const* pNode );
    virtual void dispatch ( geom const* pNode );
    virtual void dispatch ( geomFx const* pNode );
    virtual void dispatch ( group const* pNode );
    virtual void dispatch ( light const* pNode );
    virtual void dispatch ( sndEffect const* pNode );
    virtual void dispatch ( sndListener const* pNode );
    
};

/////////////////////////////////////////////////////////////////////


class simpleIsect :
  public pni::pstd::refCount
{
  public:
    typedef pni::pstd::autoRef < simpleIsect > Ref;
    typedef pni::pstd::autoRef< scene::isectTest > Test;

    simpleIsect () :
      mTest ( new scene::isectTest )
        {
        }

    void setTravMask ( unsigned int val )
        {
          mIsectDd.setTravMask ( val );
        }

    bool doIsect ( 
      pni::math::vec3 const& start, 
      pni::math::vec3 const& dir,
      float len,
      scene::node* pRoot )
        {
          mTest->initSegs ( 1 );
          mTest->setOps ( scene::isectTest::IgnoreFirstXform );
          mTest->mSegs[ 0 ].set ( start, dir, len );

          mIsectDd.setTest ( mTest.get () );
          mIsectDd.startGraph ( pRoot );
          
          return mTest->mHits[ 0 ].isValid ();
        }
        
    scene::isectTest const* getTest () const { return mTest.get (); }
    scene::isectTest::hit const& getHit ( size_t which = 0 )
        { return mTest->mHits[ 0 ]; }
    void calcHitPos ( pni::math::vec3& out )
        {
          mTest->mSegs[ 0 ].lerp ( out, mTest->mHits[ 0 ].mTval );
        }
        
    scene::nodePath const& getHitPath () const
        {
          return mTest->mHits[ 0 ].mNodePath;
        }
        
    void reset ()
        {
          mTest->clearHits ();
        }

  protected:
    ~simpleIsect () {}
  
    Test mTest;
    scene::isectDd mIsectDd;
    
    void collectRefs ( pni::pstd::refCount::Refs& refs )
        {
          refs.push_back ( mTest.get () );
        }
};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneisectdd_h


