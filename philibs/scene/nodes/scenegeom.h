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

  /** 
    This class is the container for actual geometry values (i.e., the
    positions, normals, colors, uv's, and user attributes.  It also
    contains the attribute binding information, which describes what vertex attributes
    exist in the value array, and how they are interleaved/packed.

    Note: Only float componenets are supported right now.  E.g., no GLByte
    color components, etc.
  */
class geomData :
  public pni::pstd::refCount,
  public scene::travDataContainer
{
  public:

    typedef float ValueType;
    typedef uint16_t IndexType;

    geomData () {}

      /*** Use these values for the mType member of AttributeVal to indicate which
           attribute you are specifying.  See Attribute help for an example. */
      // Note: We can only have 64 enums, because of the way these are used for
      // the 64-bit mTypesEnabled mask cache.
    enum AttributeType {
      Position,        /// Always 3 elements.
      Normal,          /// Always 3 elements.
      Color,           /// Always 4 elements.
      TCoord00,        /// Always 2 elements.
      TCoord01,        /// Always 2 elements.
      TCoord02,        /// Always 2 elements.
      TCoord03,        /// Always 2 elements.
      TCoord04,        /// Always 2 elements.
      TCoord05,        /// Always 2 elements.
      TCoord06,        /// Always 2 elements.
      TCoord07,        /// Always 2 elements.
      TCoord08,        /// Always 2 elements.
      TCoord09,        /// Always 2 elements.
      TCoord10,        /// Always 2 elements.
      TCoord11,        /// Always 2 elements.
      TCoord12,        /// Always 2 elements.
      TCoord13,        /// Always 2 elements.
      TCoord14,        /// Always 2 elements.
      TCoord15,        /// Always 2 elements.

      Attrib00,          /// User-defined
      Attrib01,          /// User-defined
      Attrib02,          /// User-defined
      Attrib03,          /// User-defined
      Attrib04,          /// User-defined
      Attrib05,          /// User-defined
      Attrib06,          /// User-defined
      Attrib07,          /// User-defined
      Attrib08,          /// User-defined
      Attrib09,           /// User-defined
      Attrib10,           /// User-defined
      Attrib11,           /// User-defined
      Attrib12,           /// User-defined
      Attrib13,           /// User-defined
      Attrib14,           /// User-defined
      Attrib15,           /// User-defined

      NumTexUnits = TCoord15 - Color,
      Undef = 0xffff

    };
  
    static_assert ( NumTexUnits == 16, "Someone changed the number of texture units" );
  
      /** Use these values as symbols for the mComponents member of AttributeVal
          when setting up attributes.  For user-defined attribute bindings, you'll
          have to set the value explicitly.  See Attributes help for an example. */
    enum AttributeComponents {
      PositionComponents = 3,
      NormalComponents = 3,  
      ColorComponents = 4,   
      TCoord00Components = 2,
      TCoord01Components = 2,
      TCoord02Components = 2,
      TCoord03Components = 2,
      TCoord04Components = 2,
      TCoord05Components = 2,
      TCoord06Components = 2,
      TCoord07Components = 2,
      TCoord08Components = 2,
      TCoord09Components = 2,
      TCoord10Components = 2,
      TCoord11Components = 2,
      TCoord12Components = 2,
      TCoord13Components = 2,
      TCoord14Components = 2,
      TCoord15Components = 2
    };
      /** From GL: GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,
           GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT are accepted by both
           functions. Additionally GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE,
           GL_INT_2_10_10_10_REV, and GL_UNSIGNED_INT_2_10_10_10_REV 
           @note Note all of these data types are supported in GLES 2 on iOS 7, etc. */
    enum DataType {
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

      /** Struct that indicates specific geometry attribute names, types, and
          sizes for driving the VBO setup process during GL evaluation. */
    struct AttributeVal
      {
        std::string mName;        /// Needed for attributes to match with vertex prog
        AttributeType mType;      /// From AttributeType enum
        DataType mDataType;       /// Data type of values in attribute from DataType
        size_t mComponents;       /// Number of floats for this attribute
        
        bool operator == ( AttributeVal const& rhs ) const
          {
            return mName == rhs.mName &&
                   mType == rhs.mType &&
                   mDataType == rhs.mDataType &&
                   mComponents == rhs.mComponents;
          }
      };

    // Attributes:
      /**
        Note: The attributes must always be packed in the values list in the
        order they occur in the interleaved values array in geomData. Generally,
        this is Positions, Normals, Colors, TCoord01, ... then User01 if used, etc.

        Note: Use the vector behavior of this class to add attribute elements, e.g.,
        attributess.push_back ( AttributeVal ( "positions", geomData::Position, geomData::PositionComponents ) ), or better, using common identifiers...
        attributess.push_back ( AttributeVal ( CommonAttributeNames[ geomData::Position ], geomData::Position, geomData::PositionComponents ) )
      */
    class attributes :
        public std::vector< AttributeVal >
      {
          typedef std::vector< AttributeVal > Base;

        public:
            /// Number of floats in vertex.
            /// @note will be pretty invalid when we support bytes, shorts, etc.
          size_t getValueStride () const
            {
              size_t ret = 0;

                // Opt: We could cache stride with a little more work in the
                // dirty functions.
              for ( auto attribute : *this )
                ret += attribute.mComponents;

              return ret;
            }

            /// Number of bytes in a vertex.
          size_t getValueStrideBytes () const
            {
                // Opt: We could cache stride with a little more work in the
                // dirty functions.
              return getValueStride() * sizeof ( ValueType );
            }

          size_t getValueOffset ( AttributeType which ) const
            {
              size_t ret = 0;

                // Opt: We could cache an array of offsets with a little more
                // work in the dirty functions.

              for ( auto attribute : *this )
              {
                if ( attribute.mType == which )
                  return ret;
                ret += attribute.mComponents;
              }

              throw std::invalid_argument( "attribute type not found" );

              return 0;
            }

          size_t getValueOffsetBytes ( AttributeType which ) const
            {
              return getValueOffset(which) * sizeof ( ValueType );  // TEMP: We might support things besides FLOAT someday!!!
            }

          bool hasAttribute ( AttributeType which ) const
            {
              clearDirty ();
              return ( 1 << which ) & mTypesEnabled;
            }

            // These methods hide std::vector methods
          void clear () { setDirty (); Base::clear (); }
          void push_back ( value_type const& val ) { setDirty(); Base::push_back ( val ); }
          void resize ( size_type val ) { setDirty (); Base::resize ( val ); }

          Base& operator = ( Base const& rhs ) = delete;
          Base& operator = ( Base&& rhs ) = delete;
          Base& operator = ( std::initializer_list< value_type > init ) = delete;

          void setDirty ( bool val = true ) { mDirty = val; }
          bool getDirty () const { return mDirty; }
          void clearDirty () const
            {
              mTypesEnabled = 0;
              for ( auto attribute : *this )
              {
                mTypesEnabled |= ( 1 << attribute.mType );
              }
              mDirty = false;
            }

        private:


          mutable bool mDirty = true;             // Only used for mTypesEnabled right now.
          mutable uint64_t mTypesEnabled = 0ULL;  // Optimizes hasAttribute calls
      };

      // This is a cheap convention hack... all classes in phi start with a
      // lower case, but typedefs upper... so this gets us consistent in
      // both ways (hah) in this class.
    typedef attributes Attributes;

      /// The values type for storing vertex attribute data.  It's always all
      ///    floats
    typedef std::vector< ValueType > Values;
      /// The indices for stringing together primitives.  It's currently always
      /// uint16_t as that's what GLES deals best with.
    typedef std::vector< IndexType > Indices;
  
      /** Note: numValues is count of floats, not count of verts.
          Generally it is numVerts * sizeof ( vert ) / sizeof ( float ). */
    void resize ( size_t numValues, size_t numIndices )
        {
          mValues.resize ( numValues );
          mIndices.resize ( numIndices );
          setDirty ();
        }
  
        /** Note: numVerts is count of verts, which is a different
            semantic than the other resize method */
    void resizeTrisWithCurrentAttributes ( size_t numVerts, size_t numTris )
        {
          mValues.resize ( numVerts * mAttributes.getValueStride () );
          size_t newSize = numTris * 3;
          mIndices.resize ( newSize );
          setDirty ();
        }
  
    size_t getVertCount () const
        { return mValues.size() / mAttributes.getValueStride(); }
        
    size_t getValueCount () const
        { return mValues.size (); }
  
    size_t getValueSizeBytes () const
        { return mValues.size () * sizeof ( ValueType ); }
  
    size_t getIndexCount () const
        { return mIndices.size (); }
        
    size_t getIndexSizeBytes () const
        { return mIndices.size () * sizeof ( IndexType ); }
  
    size_t getTriCount () const
        { return getIndexCount () / 3; }
    
      /// Return attribute data as a homogeneous vector< float > of size #getValueCount.
    Values& getValues () { return mValues; }
      /// Return attribute data as a homogeneous vector< float > of size #getValueCount.
    Values const& getValues () const { return mValues; }
    ValueType* getValuesPtr () { return &mValues[ 0 ]; }
    ValueType const* getValuesPtr () const { return &mValues[ 0 ]; }
    ValueType* getValuesPtr ( size_t index ) { return &mValues[ 0 ] + index * mAttributes.getValueStride(); }
    ValueType const* getValuesPtr ( size_t index ) const { return &mValues[ 0 ] + index * mAttributes.getValueStride(); }
  
    ValueType* getAttributePtr ( geomData::AttributeType which ) { return getValuesPtr() + mAttributes.getValueOffset ( which ); }
    ValueType const* getAttributePtr ( geomData::AttributeType which ) const { return getValuesPtr() + mAttributes.getValueOffset ( which ); }
    ValueType* getAttributePtr ( geomData::AttributeType which, size_t index ) { return getValuesPtr() + mAttributes.getValueOffset ( which ) + index * mAttributes.getValueStride(); }
    ValueType const* getAttributePtr ( geomData::AttributeType which, size_t index ) const { return getValuesPtr() + mAttributes.getValueOffset ( which ) + index * mAttributes.getValueStride(); }


    Indices& getIndices () { return mIndices; }
    Indices const& getIndices () const { return mIndices; }
    IndexType* getIndicesPtr () { return & mIndices[ 0 ]; }
    IndexType const* getIndicesPtr () const { return & mIndices[ 0 ]; }

    void setDirty ( bool dirty = true ) { mDirty = dirty; } // Used to invalidate GL-side objects like VBOs during rendering pass.
    bool getDirty () const { return mDirty; }
    void clearDirty () const { mDirty = false; }

    void setBoundsDirty () { mBounds.setIsDirty ( true ); }
    box3 const& getBounds () const { if ( mBounds.getIsDirty() ) updateBounds (); return mBounds; }

      /** @methodgroup Attribute Methods */
    void setAttributes ( Attributes const& attributes )
      { mAttributes = attributes; setDirty (); }
    Attributes const& getAttributes () const { return mAttributes; }
    Attributes& attributesOp () { setDirty (); mAttributes.setDirty(); return mAttributes; }

      /// @group helper methods

      /// @note Will create normal attribute if it isn't set, and realloc values if
      /// necessary.
      /// @note angle is currently not implemented.
      /// @param angle Breaking angle that determines if normals are blended together
      /// or create a crease.
    void generateNormals ( float degrees );
  
      /// Uniquifies any verts that are shared.  E.g., the index list will point
      /// at completely unique verts after this is done.
    void unshareVerts ();
  
      /// Share verts that have identical values.  E.g, this is handy after
      /// generating new normals, which will allow for more efficient storage
      /// of verts than a non-smooth mesh.
    void shareVerts ();
  
      /// Realloc the value array to hold more attributes without losing current values.
      /// @note Currently only handles growing the attribute list... shrinking will
      /// cause all kinds of problems right now.
    void reallocWithAttributes ( attributes const& newAttributes );
  
      /// Swap all members, including attributes, values, indices, and bounds
      /// with rhs.  This is analogous to STL swap routines.  Dirty flag will
      /// be set true for both this and rhs.
    void swap ( geomData* rhs );

      /// Affects certain geometry operations, such as shareVerts and generateNormals.
      /// @note Default is pni::math::Trait::fuzzVal, which is generally FLT_EPSILON.
    void setEpsilon ( ValueType val ) { mFuzz = val; }
    ValueType getEpsilon () const { return mFuzz; }

      /// @methodgroup Debugging Methods
    void dbg ( std::ostream& ostr = std::cout ) const;
  
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const {}
  
    void updateBounds () const;
  
  private:
      friend class geom;
  
      Attributes mAttributes;
      
      Values mValues;
      Indices mIndices;
      mutable box3 mBounds;
      ValueType mFuzz = pni::math::Trait::fuzzVal;
      mutable bool mDirty = true;
};

// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////

class geomDataXXX;

class geomDataBase
{
  protected:
    geomDataBase () = default;  // Can't instantiate this class directly.
  
      /** 
        Use these values for the mType member of AttributeVal to indicate which
        attribute you are specifying.  See Attribute help for an example.
      */
  public:
    using DirtyBounds = dirty< box3, geomDataXXX >;
  
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
  size_t const NumTexUnits = 16;
  
  
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
using geomDataXXXLtCompare =
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

class geomDataXXX :
  public geomDataBase,
  public pni::pstd::refCount,
  public geomDataBase::DirtyBounds,
  public dataIndexed< basicBinding< basicBindingItem< geomDataBase::SemanticTypes, geomDataBase::DataTypes > >,
    geomDataXXXLtCompare
  >
{
  public:
    geomDataXXX () :
        DirtyBounds { {}, &geomDataXXX::updateBounds }
      { setEpsilon( pni::math::Trait::fuzzVal ); }
  
      /// @group Geometry algorithms
    void generateNormals ( float degrees );
    void shareVerts ();
    void unshareVerts ();
  
      /// @group Storage management
    void resizeTrisWithCurrentBinding ( size_t numVerts, size_t numTris )
      { resize(numVerts, numTris * 3); }
    size_t getTriCount () const { return getIndices().size() / 3; }

      /// Bounds methods
    DirtyBounds& boundsProp () { return *this; }
    DirtyBounds const& boundsProp () const { return *this; }

    void dbg ( std::ostream& ostr ) const;

  protected:
    void updateBounds ();
  
    // Inherited
  public:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const {}


};

// ///////////////////////////////////////////////////////////////////

class geomXXX;

class geomBaseXXX
{
  public:
    using DirtyGeomData = dirty< pni::pstd::autoRef< geomDataXXX >, geomXXX >;

  protected:
    geomBaseXXX () = default; // Can't instantiate directly
};

class geomXXX :
  public node,
  public geomBaseXXX,
  public geomBaseXXX::DirtyGeomData
{
  public:
    using GeomDataRef = pni::pstd::autoRef<geomDataXXX>;
  
    DirtyGeomData& geomDataProp() { return *this; }
    DirtyGeomData const& geomDataProp () const { return *this; }

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const
      { node::collectRefs(refs); refs.push_back(geomDataProp().get()); }

    void uniquifyGeomData ()
      { if ( geomDataProp() && geomDataProp()->getNumRefs() > 1 )
          geomDataProp().set ( new geomDataXXX ( *geomDataProp().get () ) ); }

      /// @group Set bounds policy.
    using BoundsType = uint32_t;
    enum BoundsMode : BoundsType
        {
          ForceEmpty = 0x00,
          Static = 0x01,          // Updated once.
          Dynamic = 0x02,         // Updated on every change.
          Partition = 0x04,       // Generate optimized structure.
          NumBoundsModes,
          Default = Dynamic
        };

    void setBoundsMode ( int bmode )
        { setGeomBoundsDirty (); mBoundsMode = bmode; }
    int getBoundsMode () const { return mBoundsMode; }

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
};

// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////

class geom :
  public node
{
  public:
    typedef geomData::Values Values;
    typedef geomData::Indices Indices;
    typedef geomData::AttributeType AttributeType;
    typedef geomData::AttributeComponents AttributeComponents;
    typedef geomData::AttributeVal AttributeVal;
    typedef geomData::Attributes Attributes;
  
     geom();
//     virtual ~geom();
//     geom(geom const& rhs);
//     geom& operator=(geom const& rhs);
//      bool operator==(geom const& rhs) const;

      // Node framework methods.      
    virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }
    virtual geom* dup () const { return new geom ( *this ); }

    
    typedef pni::pstd::autoRef< geomData > GeomDataRef;
    
      /** @methodgroup Geometry Methods
          Efficiently reference geometry data so that dup operations can
          be lightweight.  E.g., geometry instancing. */

      /**
          Handy function that sets bounds dirty and returns geom data.
          LAME: When geom data is shared, it's bounds will be calc'd
          for every parent... this could be optimized by putting the
          actual bounds in the geomData class.  No biggie, though.
          Update: geomData now has bounds and updates only once, regardless
          of multiple parentage. */
    void setGeomData ( geomData* pData ) { mGeomData = pData; setGeomBoundsDirty (); }
    geomData* getGeomData () { return mGeomData.get (); }
    geomData const* getGeomData () const { return mGeomData.get (); }
    geomData* geometryOp () { setGeomBoundsDirty (); return mGeomData.get (); }
        
      ///////
    void uniquifyGeometry ();

      /** @methodgroup Bounds Methods */

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
    
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
    
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
    typedef geomData::ValueType ValueType;

    geom::GeomDataRef mGdata;
    size_t mStride;
    ValueType* mCur;
    ValueType* mEnd;
    
    void calcEnd ()
        {
          mEnd = mCur;
          mEnd += mGdata->getValues ().size ();        
        }

  public:
    vertIter ( geom* pGeom ) :
      mGdata ( pGeom->getGeomData () ),
      mStride ( mGdata->getAttributes ().getValueStride () ),
      mCur ( &mGdata->getValues ()[ 0 ] )
        {
          calcEnd ();
        }
        
    vertIter ( geomData* pGdata ) :
      mGdata ( pGdata ),
      mStride ( mGdata->getAttributes ().getValueStride () ),
      mCur ( &mGdata->getValues ()[ 0 ] )
        {
          calcEnd ();
        }
        
    geomData* operator->() const { return mGdata.get (); }
    ValueType* operator& () { return mCur; }
    ValueType& operator* () const { return *mCur; }
    ValueType& operator[] ( size_t val ) const { return mCur[ val ]; }
    ValueType* operator () ( size_t val ) const { return &mCur[ val * mStride ]; }

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


class vertIterXXX
{
    typedef geomDataXXX::ValueType ValueType;

    geomDataXXX* mGdata;
    size_t mStride;
    ValueType* mCur;
    ValueType* mEnd;
    
    void calcEnd ()
        {
          mEnd = mCur;
          mEnd += mGdata->sizeBytes();
        }

  public:
    vertIterXXX ( geomXXX* pGeom ) :
      vertIterXXX { pGeom->geomDataProp().get() } {}
        
    vertIterXXX ( geomDataXXX* pGdata ) :
      mGdata ( pGdata ),
      mStride ( mGdata->mBinding.getValueStrideBytes () ),
      mCur ( mGdata->getPtr<ValueType>() )
        {
          calcEnd ();
        }

/*
    geomDataXXX* operator->() const { return mGdata.get (); }
    ValueType* operator& () { return mCur; }
    ValueType& operator* () const { return *mCur; }
    ValueType& operator[] ( size_t val ) const { return mCur[ val ]; }
    ValueType* operator () ( size_t val ) const { return &mCur[ val * mStride ]; }
*/

    ValueType* operator& () = delete;
    ValueType const* operator& () const = delete;

    template< class Type >
    Type& get ( size_t offset ) { return *reinterpret_cast< Type* >( mCur + offset ); }
    template< class Type >
    Type const& get ( size_t offset ) const { return *reinterpret_cast< Type* >( mCur + offset ); }

      /// Increments to next full vertex (i.e., stride), not next float value.
    vertIterXXX& operator++ ()
        {
          mCur += mStride;
          return *this;
        }

      /// Increments to next full vertex (i.e., stride), not next float value.
    vertIterXXX& operator+= ( size_t val )
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
    typedef geomData::ValueType ValueType;
    
    geom::GeomDataRef mGdata;
    size_t mStride;
    size_t mCur;
    ValueType* mPtr;
    
    ValueType* getPtr ( size_t val ) const
        {
          size_t ind = mGdata->getIndices ()[ val ];
          return &mGdata->getValues ()[ ind * mStride ];        
        }

  public:
    triIter ( geom* pGeom ) :
      mGdata ( pGeom->getGeomData () ),
      mStride ( mGdata->getAttributes ().getValueStride () ),
      mCur ( 0 )
        {
          mPtr = getPtr ( 0 );
        }
        
    triIter ( geomData* pGdata ) :
      mGdata ( pGdata ),
      mStride ( mGdata->getAttributes ().getValueStride () ),
      mCur ( 0 )
        {
          mPtr = getPtr ( 0 );
        }
        
    geomData* operator->() const { return mGdata.get (); }
    ValueType* operator& () { return mPtr; }
    ValueType& operator* () const { return *mPtr; }
    ValueType& operator[] ( size_t val ) const { return mPtr[ val ]; }
    
    /// Relative vertex address operator.  If mCur is 3, calling this with 2
    /// will give the starting pointer for vertex 5.
    ValueType* operator () () const { return mPtr; }
    ValueType* operator () ( size_t val ) const { return getPtr ( mCur + val ); }

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
        
    operator bool ()
        {
          return mCur < mGdata->getIndices ().size ();
        }
    
};

class triIterXXX
{
    typedef geomDataXXX::ValueType ValueType; // uint8_t
    
    geomDataXXX* mGdata;
    size_t mStrideBytes;
    size_t mCur;
    ValueType* mPtr;
    
    ValueType* getPtr ( size_t val ) const
        {
          size_t ind = mGdata->getIndices ()[ val ];
          return &mGdata->getPtr< ValueType >()[ ind * mStrideBytes ];
        }

  public:
    triIterXXX ( geomDataXXX* pGdata ) :
      mGdata ( pGdata ),
      mStrideBytes ( mGdata->mBinding.getValueStrideBytes() ),
      mCur ( 0 )
        {
          mPtr = getPtr ( 0 );
        }

    triIterXXX ( geomXXX* pGeom ) :
      triIterXXX ( pGeom->geomDataProp().get() ) {}
  
    template< class Type >
    Type& get ( size_t offset ) { return *reinterpret_cast< Type* >( mPtr + offset ); }
    template< class Type >
    Type const& get ( size_t offset ) const { return *reinterpret_cast< Type* >( mPtr + offset ); }
  
/*
    geomData* operator->() const { return mGdata.get (); }
    ValueType* operator& () { return mPtr; }
    ValueType& operator* () const { return *mPtr; }
    ValueType& operator[] ( size_t val ) const { return mPtr[ val ]; }
    
      /// Relative vertex address operator.  If mCur is 3, calling this with 2
      /// will give the starting pointer for vertex 5.
    ValueType* operator () () const { return mPtr; }
    ValueType* operator () ( size_t val ) const { return getPtr ( mCur + val ); }
*/

    ValueType* operator& () = delete;
    ValueType const* operator& () const = delete;

      /// Get the current index into the indices array.
    geomData::IndexType getCurIndex () const { return mCur; }
      /// Increments to next full vertex (i.e., stride), not next float value.
      /// Apps need to keep track of their own mod 3 arithmetic.
    triIterXXX& operator++ ()
        {
          mCur++;
          mPtr = getPtr ( mCur );
          return *this;
        }

      /// Increments to next full vertex (i.e., stride), not next float value.
      /// Apps need to keep track of their own mod 3 arithmetic.
    triIterXXX& operator+= ( size_t val )
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


