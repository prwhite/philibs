/////////////////////////////////////////////////////////////////////
//
//    class: geom
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenegeom_h
#define scenescenegeom_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include "scenenode.h"
#include "scenegraphdd.h"
#include <vector>
#include <cassert>

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class geomData :
  public pni::pstd::refCount,
  public scene::travDataContainer
{
  public:

    typedef unsigned short SizeType;

    geomData () : 
      mBindings ( Positions ),
      mDirty ( true )
        {}

    // Bindings:
    // Note: The bindings must always be packed in the
    // values array in the order indicated in the enum
    // below, i.e., positions first, normals second...
    // and if no normals, then TCoords0 is second, etc.
    enum Bindings {
      Positions = 0,  // Always 3 elements.
      Normals = 1,    // Always 3 elements.
      Colors = 2,     // Always 4 elements.
      TCoords0 = 4,   // Always 2 elements.
      TCoords1 = 8    // Always 2 elements.
    };
    
    void setBindings ( unsigned int val = Positions )
        {
          mBindings = val;
          setDirty ();
        }
    
    unsigned int getBindings () const { return mBindings; }
    
    SizeType getValueStride () const
        {
          SizeType val = 3; // For positions
          if ( mBindings & Normals ) val += 3;
          if ( mBindings & Colors ) val += 4;
          if ( mBindings & TCoords0 ) val += 2;
          if ( mBindings & TCoords1 ) val += 2;
          return val;
        }
    
     size_t getValueOffset ( Bindings which ) const
        {
          size_t val = 0;
          if ( which == Positions )
            return 0;

          val += 3;

          if ( which == Colors )
            return val;

          if ( mBindings & Colors )
            val += 4;

          if ( which == Normals )
            return val;

          if ( mBindings & Normals )
            val += 3;
          
          if ( which == TCoords0 )
            return val;

          if ( mBindings & TCoords0 )
            val += 2;

          if ( which == TCoords1 )
            return val;

          return 0;
        }

    /////////////////////////////////////////////////////////////////
    class indexVec :
      public std::vector< SizeType >
    {
        typedef std::vector< SizeType > Base;
        enum { Buff = 0 };
        
      public:
        void resize ( size_type size, SizeType val = 0 )
            {
              Base::resize ( size + Buff, val );
            }
        
        void reserve ( size_type val ) { Base::reserve ( val + Buff ); }
            
        size_type size () const { return Base::size () - Buff; }
        
        bool empty () const { return Base::size () - Buff == 0; }
    };
    
    typedef std::vector< float > Values;
//    typedef std::vector< SizeType > Indices;
    typedef indexVec Indices;
    
      // Note: values is count of floats, not count of verts.
      // Generally it is numVerts * sizeof ( vert ) / sizeof ( float ).
    void resize ( SizeType values, SizeType indices )
        {
          mValues.resize ( values );
          mIndices.resize ( indices );
          setDirty ();
        }
        
    void resizeTrisWithBinding ( SizeType values, SizeType numTris )
        {
          mValues.resize ( values * getValueStride () );
          size_t newSize = numTris * 3;
          mIndices.resize ( newSize );
          setDirty ();
        }
        
    SizeType getValueCount () const 
        { return static_cast< SizeType > ( mValues.size () ); }
    
    SizeType getElemCount () const 
        { return static_cast< SizeType > ( mIndices.size () ); }
        
    SizeType getTriCount () const
        { return getElemCount () / 3; }
    
    Values& getValues () { return mValues; }
    Values const& getValues () const { return mValues; }
    
    Indices& getIndices () { return mIndices; }
    Indices const& getIndices () const { return mIndices; }
  
    void setDirty () { mDirty = true; } // Used to invalidate GL-side objects like VBOs during rendering pass.
    void setBoundsDirty () { mBounds.setIsDirty ( true ); }
    box3 const& getBounds () const { if ( mBounds.getIsDirty() ) updateBounds (); return mBounds; }
  
    void dbg ();
    
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) {}
  
    void updateBounds () const;
  
  private:
      friend class geom;
  
      unsigned int mBindings;
      
      Values mValues;
      Indices mIndices;
      mutable box3 mBounds;
      bool mDirty;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class geom :
  public node
{
  public:
    typedef geomData::Values Values;
    typedef geomData::Indices Indices;
    typedef geomData::Bindings Bindings;
    typedef geomData::SizeType SizeType;
  
     geom();
//     virtual ~geom();
//     geom(geom const& rhs);
//     geom& operator=(geom const& rhs);
//      bool operator==(geom const& rhs) const;

      // Node framework methods.      
    virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }
    virtual node* dup () const { return new geom ( *this ); }

    
      // Efficiently reference geometry data so that dup operations can
      // be lightweight.
      // NOTE: Public data!
    typedef pni::pstd::autoRef< geomData > GeomDataRef;
    
      // Handy function that sets bounds dirty and returns geom data.
      // LAME: When geom data is shared, it's bounds will be calc'd
      // for every parent... this could be optimized by putting the
      // actual bounds in the geomData class.  No biggie, though.
    void setGeomData ( geomData* pData ) { mGeomData = pData; setGeomBoundsDirty (); }
    geomData* getGeomData () { return mGeomData.get (); }
    geomData const* getGeomData () const { return mGeomData.get (); }
    geomData* geometryOp () { setGeomBoundsDirty (); return mGeomData.get (); }
        
      ///////
    void uniquifyGeometry ();
    
      // Bounding geometry management hints.
      // TODO: Not all of these are implemented.  
    enum BoundsMode
        {
          ForceEmpty = 0x00,
          Static = 0x01,          // Updated once.
          Dynamic = 0x02,         // Updated on every change.
          Partition = 0x04,       // Generate optimized structure.
          Default = Dynamic
        };

    void setBoundsMode ( int bmode ) 
        { setGeomBoundsDirty (); mBoundsMode = bmode; }
    int getBoundsMode () const { return mBoundsMode; }
    
    virtual void setGeomBoundsDirty ();
    
  protected:
        // Also part of node framework.
    virtual void updateBounds () const;
    virtual void generateGeomBounds () const;
    virtual void generateGeomPartition () const;
    
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
    
    GeomDataRef mGeomData;
    int mBoundsMode;
    mutable bool mGeomBoundsDirty;
    
  private:

};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class vertIter
{
    typedef geomData::SizeType SizeType;

    geom::GeomDataRef mGdata;
    SizeType mStride;
    float* mCur;
    float* mEnd;
    
    void calcEnd ()
        {
          mEnd = mCur;
          mEnd += mGdata->getValues ().size ();        
        }

  public:
    vertIter ( geom* pGeom ) :
      mGdata ( pGeom->getGeomData () ),
      mStride ( mGdata->getValueStride () ),
      mCur ( &mGdata->getValues ()[ 0 ] )
        {
          calcEnd ();
        }
        
    vertIter ( geomData* pGdata ) :
      mGdata ( pGdata ),
      mStride ( mGdata->getValueStride () ),
      mCur ( &mGdata->getValues ()[ 0 ] )
        {
          calcEnd ();
        }
        
    geomData* operator->() const { return mGdata.get (); }
    float* operator& () { return mCur; }
    float& operator* () const { return *mCur; }
    float& operator[] ( size_t val ) const { return mCur[ val ]; }
    float* operator () ( size_t val ) const { return &mCur[ val * mStride ]; }

      // Increments to next full vertex (i.e., stride), not next float value.
    vertIter& operator++ ()
        {
          mCur += mStride;
          return *this;
        }

      // Increments to next full vertex (i.e., stride), not next float value.
    vertIter& operator+= ( size_t val )
        {
          mCur += mStride * val;
          return *this;
        }
        
    operator bool ()
        {
          return mCur < mEnd;
        }
    
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class triIter
{
    typedef geomData::SizeType SizeType;
    
    geom::GeomDataRef mGdata;
    SizeType mStride;
    size_t mCur;
    float* mPtr;
    
    float* getPtr ( size_t val ) const
        {
          size_t ind = mGdata->getIndices ()[ val ];
          return &mGdata->getValues ()[ ind * mStride ];        
        }

  public:
    triIter ( geom* pGeom ) :
      mGdata ( pGeom->getGeomData () ),
      mStride ( mGdata->getValueStride () ),
      mCur ( 0 )
        {
          mPtr = getPtr ( 0 );
        }
        
    triIter ( geomData* pGdata ) :
      mGdata ( pGdata ),
      mStride ( mGdata->getValueStride () ),
      mCur ( 0 )
        {
          mPtr = getPtr ( 0 );
        }
        
    geomData* operator->() const { return mGdata.get (); }
    float* operator& () { return mPtr; }
    float& operator* () const { return *mPtr; }
    float& operator[] ( size_t val ) const { return mPtr[ val ]; }
    
    // Relative vertex address operator.  If mCur is 3, calling this with 2 
    // will give the starting pointer for vertex 5.
    float* operator () () const { return mPtr; }
    float* operator () ( size_t val ) const { return getPtr ( mCur + val ); }

    // Increments to next full vertex (i.e., stride), not next float value.
    // Apps need to keep track of their own mod 3 arithmetic.
    triIter& operator++ ()
        {
          mCur++;
          mPtr = getPtr ( mCur );
          return *this;
        }

    // Increments to next full vertex (i.e., stride), not next float value.
    // Apps need to keep track of their own mod 3 arithmetic.
    triIter& operator+= ( size_t val )
        {
          mCur += val;
          mPtr = getPtr ( mCur );
          return *this;
        }
        
    operator bool ()
        {
          return mCur < mGdata->getIndices ().size ();
        }
    
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenegeom_h


