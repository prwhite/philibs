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
#include <iostream>
#include <tuple>
#include "pnidbg.h"

#include "scenedirty.h"
#include "scenedata.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////

class geomData;

class geomDataBase
{
  protected:
    geomDataBase () = default;  // Can't instantiate this class directly.

      /**
        Use these values for the mType member of AttributeVal to indicate which
        attribute you are specifying.  See Attribute help for an example.
      */
  
  public:
    using DirtyBounds = dirtyProp< box3, geomData >;
    using DirtyGeomData = dirtyProp< geomData, geomData >;
  
    enum SemanticTypes
    {
      Position,
      Normal,
      Color,
      TCoord,
      User
    };
  
      /** 
        From GL: GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,
        GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT are accepted by both
        functions. Additionally GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE,
        GL_INT_2_10_10_10_REV, and GL_UNSIGNED_INT_2_10_10_10_REV
        @note Note all of these data types are supported in GLES 2 on iOS 7, etc.
      */
    enum DataTypes
    {
      DataType_BYTE,
      DataType_UNSIGNED_BYTE,
      DataType_SHORT,
      DataType_UNSIGNED_SHORT,
      DataType_INT,
      DataType_UNSIGNED_INT,
      DataType_HALF_FLOAT,
      DataType_FLOAT,
      DataType_DOUBLE,
      DataType_INT_2_10_10_10_REV,
      DataType_UNSIGNED_INT_2_10_10_10_REV
    };

      /** 
        Use these values as symbols for the mComponents member of AttributeVal
        when setting up attributes.  For user-defined attribute bindings, you'll
        have to set the value explicitly.  See Attributes help for an example.
      */
    enum AttributeComponents
    {
      PositionComponents = 3,
      NormalComponents = 3,  
      ColorComponents = 4,   
      TCoordComponents = 2
    };

    /// @group Miscelaneaous constants
    // Maybe...
  static size_t const NumTexUnits = 16;
  
  
};

// ///////////////////////////////////////////////////////////////////
// These data types are only used for casting and comparison...
// they are not full-fledged types.
// TODO: These types also need << ostream operators to work with lt_variadic

  // TODO: Finish this out, etc... move it to its own header.
  // @see IEEE_754-2008
struct lt_float16_t
{
  uint16_t mVal;
  
  bool operator < ( lt_float16_t const& rhs ) const { return false; }
};

  // TODO: Finish this out, etc... move it to its own header.
  // @see http://www.informit.com/articles/article.aspx?p=2033340&seqNum=3
struct lt_int2101010_rev_t
{
  int32_t mVal;
  
  bool operator < ( lt_int2101010_rev_t const& rhs ) const { return false; }
};

  // unsigned 2_10_10_10 can be compared as an int, unlike signed above.
  // @see http://www.informit.com/articles/article.aspx?p=2033340&seqNum=3
struct lt_uint2101010_rev_t
{
  uint32_t mVal;
  
  bool operator < ( lt_uint2101010_rev_t const& rhs ) const { return mVal < rhs.mVal; }
};

  // Defines the dispatch table used when comparing vertex elements in the
  // values vector.
using geomDataLtCompare =
    lt_variadic<
        lt_pair<geomDataBase::DataType_BYTE, int8_t>,
        lt_pair<geomDataBase::DataType_UNSIGNED_BYTE, uint8_t>,
        lt_pair<geomDataBase::DataType_SHORT, int16_t>,
        lt_pair<geomDataBase::DataType_UNSIGNED_SHORT, uint16_t>,
        lt_pair<geomDataBase::DataType_INT, int32_t>,
        lt_pair<geomDataBase::DataType_UNSIGNED_INT, uint32_t>,
//        lt_pair<geomDataBase::DataType_INT_2_10_10_10_REV, lt_int2101010_rev_t>,
//        lt_pair<geomDataBase::DataType_UNSIGNED_INT_2_10_10_10_REV, lt_uint2101010_rev_t>,
//        lt_pair<geomDataBase::DataType_HALF_FLOAT, lt_float16_t>,
        lt_pair<geomDataBase::DataType_FLOAT, float>,
        lt_pair<geomDataBase::DataType_DOUBLE, double>
    >;

// ///////////////////////////////////////////////////////////////////

class geomData :
  public geomDataBase,
  public pni::pstd::refCount,
  public geomDataBase::DirtyBounds,
  public geomDataBase::DirtyGeomData,
  public travDataContainer,
  public dataIndexed< basicBinding< basicBindingItem< geomDataBase::SemanticTypes, geomDataBase::DataTypes > >,
    geomDataLtCompare
  >
{
  public:
    geomData () :
        DirtyBounds { 0, 0, &geomData::updateBounds },
        DirtyGeomData { 0, 0 }
      { setEpsilon ( pni::math::Trait::fuzzVal ); }
  
      /// @group Geometry algorithms
    void generateNormals ( float degrees );
    void shareVerts ();
    void unshareVerts ();
  
      /// @group Storage management
    void resizeTrisWithCurrentBinding ( size_t numVerts, size_t numTris )
      { resize(numVerts, numTris * 3); }
    size_t getTriCount () const { return getIndices().size() / 3; }

      /// @group Bounds "dirty" accessors.
    DirtyBounds& boundsProp () { return *this; }
    DirtyBounds const& boundsProp () const { return *this; }

    using DirtyGeomData::op;
    using DirtyGeomData::setDirty;
    using DirtyGeomData::getDirty;
    using DirtyGeomData::clearDirty;
    using DirtyGeomData::get;
    using DirtyGeomData::getUpdated;

    void dbg ( std::ostream& ostr ) const;
  
  protected:
    void updateBounds () const;
  
    // Inherited
  public:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const {}


};

// ///////////////////////////////////////////////////////////////////

class geom :
  public node
{
  public:
    using GeomDataRef = pni::pstd::autoRef<geomData>;
  
    void setGeomData ( geomData* pData ) { mGeomData = pData; setGeomBoundsDirty(); }
    geomData* getGeomData() { return mGeomData.get(); }
    geomData const* getGeomData () const { return mGeomData.get(); }

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const
      { node::collectRefs(refs); refs.push_back(mGeomData.get()); }

    void uniquifyGeomData ()
      { if ( mGeomData && mGeomData->getNumRefs() > 1 )
          mGeomData = ( new geomData ( *mGeomData ) ); }

      /// @group Set bounds policy.
    using BoundsType = uint32_t;
    enum BoundsMode : BoundsType
        {
          ForceEmpty = 0x00,      /// Make bounds always empty.
          Static = 0x01,          /// Updated once when first queried.
          Dynamic = 0x02,         /// Updated on every change.
          Partition = 0x04,       /// Generate optimized structure.
          NumBoundsModes,
          Default = Dynamic
        };

    void setBoundsMode ( int bmode )
        { setGeomBoundsDirty (); mBoundsMode = bmode; }
    int getBoundsMode () const { return mBoundsMode; }

      // @group Node framework methods.
    virtual void accept ( graphDd* pDd ) const override { pDd->dispatch ( this ); }
    virtual geom* dup () const override { return new geom ( *this ); }

    /// @group Bounds query and update helpers.
  public:
    virtual void setGeomBoundsDirty ();
  
  protected:
    virtual void updateBounds () const override;
    virtual void generateGeomBounds () const;
    virtual void generateGeomPartition () const;
  
  private:
    static_assert(NumBoundsModes < INT_MAX, "bounds mode doesn't fit in storage type");
    BoundsType mBoundsMode = Default;
    GeomDataRef mGeomData;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class vertIter
{
    typedef geomData::ValueType ValueType;

    geomData* mGdata;
    size_t mStride;
    ValueType* mCur;
    ValueType* mEnd;
    
    void calcEnd ()
        {
          mEnd = mCur;
          mEnd += mGdata->sizeBytes();
        }

  public:
    vertIter ( geom* pGeom ) :
      vertIter { pGeom->getGeomData() } {}
        
    vertIter ( geomData* pGdata ) :
      mGdata ( pGdata ),
      mStride ( mGdata->mBinding.getValueStrideBytes () ),
      mCur ( mGdata->getPtr<ValueType>() )
        {
          calcEnd ();
        }

    ValueType* operator& () = delete;
    ValueType const* operator& () const = delete;

    template< class Type >
    Type& get ( size_t offset ) { return *reinterpret_cast< Type* >( mCur + offset ); }
    template< class Type >
    Type const& get ( size_t offset ) const { return *reinterpret_cast< Type* >( mCur + offset ); }

    template< class Type >
    Type& get ( size_t element, size_t offset ) { return *reinterpret_cast< Type* >( mCur + element * mStride + offset ); }
    template< class Type >
    Type const& get ( size_t element, size_t offset ) const { return *reinterpret_cast< Type* >( mCur + element * mStride + offset ); }

      /// Increments to next full vertex (i.e., stride), not next float value.
    vertIter& operator++ ()
        {
          mCur += mStride;
          return *this;
        }

      /// Increments to next full vertex (i.e., stride), not next float value.
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
    typedef geomData::ValueType ValueType; // uint8_t
    
    geomData* mGdata;
    size_t mStrideBytes;
    size_t mCur;
    ValueType* mPtr;
    
    ValueType* getPtr ( size_t val ) const
        {
          size_t ind = mGdata->getIndices ()[ val ];
          return &mGdata->getPtr< ValueType >()[ ind * mStrideBytes ];
        }

  public:
    triIter ( geomData* pGdata ) :
      mGdata ( pGdata ),
      mStrideBytes ( mGdata->mBinding.getValueStrideBytes() ),
      mCur ( 0 )
        {
          mPtr = getPtr ( 0 );
        }

    triIter ( geom* pGeom ) :
      triIter ( pGeom->getGeomData() ) {}
  
      /// @group Access current pointer plus some offset, typically based on
      /// #geomData.mBinding.getValueOffsetBytes(<some semantic type id>).
    template< class Type >
    Type& get ( size_t offset ) { return *reinterpret_cast< Type* >( mPtr + offset ); }
    template< class Type >
    Type const& get ( size_t offset ) const { return *reinterpret_cast< Type* >( mPtr + offset ); }

      /// @group Relative, random access similar to #get with a simple offset.
      /// Will get nth element +- from current iteration point, plus semantic offset.
    template< class Type >
    Type& get ( size_t element, size_t offset ) { return *reinterpret_cast< Type* >( getPtr( element ) + offset ); }
    template< class Type >
    Type const& get ( size_t element, size_t offset ) const { return *reinterpret_cast< Type* >( getPtr( element ) + offset ); }

  
    ValueType* operator& () = delete;
    ValueType const* operator& () const = delete;

      /// Get the current index into the indices array.
    geomData::IndexType getCurIndex () const { return mCur; }
      /// Increments to next full vertex (i.e., stride), not next float value.
      /// Apps need to keep track of their own mod 3 arithmetic.
    triIter& operator++ ()
        {
          mCur++;
          mPtr = getPtr ( mCur );
          return *this;
        }

      /// Increments to next full vertex (i.e., stride), not next float value.
      /// Apps need to keep track of their own mod 3 arithmetic.
    triIter& operator+= ( size_t val )
        {
          mCur += val;
          mPtr = getPtr ( mCur );
          return *this;
        }

    triIter& operator-= ( size_t val )
        {
          mCur -= val;
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


