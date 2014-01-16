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

    typedef uint16_t SizeType;

    geomData () {}

      /*** Use these values for the mType member of AttributeVal to indicate which
           attribute you are specifying.  See Attribute help for an example. */
      // Note: We can only have 64 enums, because of the way these are used for
      // the 64-bit mTypesEnabled mask cache.
    enum AttributeType : SizeType {
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

      Undef = 0xffff

    };
      /** Use these values as symbols for the mComponents member of AttributeVal
          when setting up attributes.  For user-defined attribute bindings, you'll
          have to set the value explicitly.  See Attributes help for an example. */
    enum AttributeComponents : SizeType {
      PositionsComponents = 3,
      NormalsComponents = 3,  
      ColorsComponents = 4,   
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
    enum DataType : SizeType {
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
    struct AttributeVal final
      {
        std::string mName;        /// Only needed for user-defined attributes to match with vertex prog
        AttributeType mType;      /// From AttributeType enum
        DataType mDataType;       /// Data type of values in attribute from DataType
        SizeType mComponents;     /// Number of floats for this attribute
      };

    // Attributes:
      /**
        Note: The attributes must always be packed in the values list in the
        order they occur in the interleaved values array in geomData. Generally,
        this is Positions, Normals, Colors, TCoord01, ... then User01 if used, etc.

        Note: Use the vector behavior of this class to add attribute elements, e.g.,
        attributess.push_back ( AttributeVal ( "positions", geomData::Positions, geomData::PositionsComponents ) ), or better, using common identifiers...
        attributess.push_back ( AttributeVal ( CommonAttributeNames[ geomData::Positions ], geomData::Positions, geomData::PositionsComponents ) )
      */
    class attributes :
        public std::vector< AttributeVal >
      {
          typedef std::vector< AttributeVal > Base;

        public:
          SizeType getValueStride () const
            {
              SizeType ret = 0;

                // Opt: We could cache stride with a little more work in the
                // dirty functions.
              for ( auto attribute : *this )
                ret += attribute.mComponents;

              return ret;
            }

          SizeType getValueStrideBytes () const
            {
                // Opt: We could cache stride with a little more work in the
                // dirty functions.
              return getValueStride() * sizeof ( float );
            }

          SizeType getValueOffset ( AttributeType which ) const
            {
              SizeType ret = 0;

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

          SizeType getValueOffsetBytes ( AttributeType which ) const
            {
              return getValueOffset(which) * sizeof ( float );  // TEMP: We might support things besides FLOAT someday!!!
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
            }

        private:


          mutable bool mDirty = true;             // Only used for mTypesEnabled right now.
          mutable uint64_t mTypesEnabled = 0ULL;  // Optimizes hasAttribute calls
      };

      // This is a cheap convention hack... all classes in phi start with a
      // lower case, but typedefs upper... so this gets us consistent in
      // both ways in this class.
    typedef attributes Attributes;

    /////////////////////////////////////////////////////////////////
      /** The vector that contains indices for the geomData object */
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

      /** The values type for storing vertex attribute data.  It's always all
          floats */
    typedef std::vector< float > Values;
    typedef indexVec Indices;
    
      /** Note: numValues is count of floats, not count of verts.
          Generally it is numVerts * sizeof ( vert ) / sizeof ( float ). */
    void resize ( SizeType numValues, SizeType numIndices )
        {
          mValues.resize ( numValues );
          mIndices.resize ( numIndices );
          setDirty ();
        }
  
        /** Note: numVerts is count of verts, which is a different
            semantic than the other resize method */
    void resizeTrisWithCurrentAttributes ( SizeType numVerts, SizeType numTris )
        {
          mValues.resize ( numVerts * mAttributes.getValueStride () );
          size_t newSize = numTris * 3;
          mIndices.resize ( newSize );
          setDirty ();
        }
        
    SizeType getValueCount () const 
        { return static_cast< SizeType > ( mValues.size () ); }
  
    SizeType getValueSizeBytes () const
        { return mValues.size () * sizeof ( float ); }
  
    SizeType getIndexCount () const
        { return static_cast< SizeType > ( mIndices.size () ); }
        
    SizeType getIndexSizeBytes () const
        { return mIndices.size () * sizeof ( SizeType ); }
  
    SizeType getTriCount () const
        { return getIndexCount () / 3; }
    
    Values& getValues () { return mValues; }
    Values const& getValues () const { return mValues; }
    
    Indices& getIndices () { return mIndices; }
    Indices const& getIndices () const { return mIndices; }
  
    void setDirty () { mDirty = true; } // Used to invalidate GL-side objects like VBOs during rendering pass.
    bool getDirty () const { return mDirty; }
    void clearDirty () const { mDirty = false; }

    void setBoundsDirty () { mBounds.setIsDirty ( true ); }
    box3 const& getBounds () const { if ( mBounds.getIsDirty() ) updateBounds (); return mBounds; }

      /** @methodgroup Attribute Methods */
    void setAttributes ( Attributes const& attributes )
      { mAttributes = attributes; setDirty (); }
    Attributes const& getAttributes () const { return mAttributes; }
    Attributes& attributesOp () { setDirty (); mAttributes.setDirty(); return mAttributes; }

      /** @methodgroup Debugging Methods */
    void dbg ();
    
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) {}
  
    void updateBounds () const;
  
  private:
      friend class geom;
  
      Attributes mAttributes;
      
      Values mValues;
      Indices mIndices;
      mutable box3 mBounds;
      mutable bool mDirty;
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
    typedef geomData::AttributeType AttributeType;
    typedef geomData::AttributeComponents AttributeComponents;
    typedef geomData::AttributeVal AttributeVal;
    typedef geomData::Attributes Attributes;
    typedef geomData::SizeType SizeType;
  
     geom();
//     virtual ~geom();
//     geom(geom const& rhs);
//     geom& operator=(geom const& rhs);
//      bool operator==(geom const& rhs) const;

      // Node framework methods.      
    virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }
    virtual node* dup () const { return new geom ( *this ); }

    
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


