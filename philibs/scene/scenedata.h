// ///////////////////////////////////////////////////////////////////
//
//  class: data
//
// ///////////////////////////////////////////////////////////////////

#ifndef scenedata_h
#define scenedata_h

// ///////////////////////////////////////////////////////////////////

#include <vector>
#include <cassert>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "pnipstd.h"
#include "sceneltcompare.h"

// ///////////////////////////////////////////////////////////////////

namespace scene {

// ///////////////////////////////////////////////////////////////////

enum BasicSemanticTypes {

  Position,
  Normal,
  Color,
  
  TexCoord,
  User,
  
  semantic_type_count
};

size_t const BasicSemanticTypesCount[] {

  3,
  3,
  4,
  
  2,
  4
};

enum BasicDataTypes {
  type_uint8_t,
  type_uint16_t,
  type_uint32_t,

  type_int8_t,
  type_int16_t,
  type_int32_t,
  
  type_float16_t,
  type_float32_t,
  type_float64_t,

  data_type_count
};

size_t const BasicDataTypesSizes[] = {
  1,
  2,
  4,
  
  1,
  2,
  4,
  
  2,
  4,
  8
};

using BasicLtCompare = lt_variadic<
  lt_pair<type_uint8_t, uint8_t>,
  lt_pair<type_uint16_t, uint16_t>,
  lt_pair<type_uint32_t, uint32_t>,
  lt_pair<type_int8_t, int8_t>,
  lt_pair<type_int16_t, int16_t>,
  lt_pair<type_int32_t, int32_t>,
//  lt_pair<type_float16_t, float16_t>,  // TODO: float16_t
  lt_pair<type_float32_t, float>,
  lt_pair<type_float64_t, double>
>;

static_assert ( sizeof ( BasicSemanticTypesCount ) / sizeof ( size_t ) == semantic_type_count,
    "BasicSemanticTypes and BasicSemanticTypesCount out of sync" );

static_assert ( sizeof ( BasicDataTypesSizes ) / sizeof ( size_t ) == data_type_count,
    "BasicDataTypes and BasicDataTypesSizes out of sync" );

// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////

#define SCENEDATALTCOMPAREHELPER(lhs,rhs) if ( lhs != rhs ) return lhs < rhs

template< typename SemanticType_, typename DataType_ >
struct basicBindingItem
{
  using SemanticType = SemanticType_;
  using DataType = DataType_;

    /// @group Public data!!!
  std::string       mName;              /// Name of bound attribute
  SemanticType      mSemanticType;      /// Semantic type represented
  DataType          mDataType;          /// Data type represented
  size_t            mSize = 0;          /// Size in bytes for each element
  size_t            mCount = 0;         /// Number of elements for this binding
  size_t            mIndex = 0;         /// E.g., which texture unit/user field, or ignored
  
  basicBindingItem () {}
  
    /// @group constructors
  basicBindingItem (
      std::string const& name,
      SemanticType semanticType,
      DataType dataType,
      size_t size,
      size_t count,
      size_t index = 0 ) :
    mName ( name ),
    mSemanticType ( semanticType ),
    mDataType ( dataType ),
    mSize ( size ),
    mCount ( count ),
    mIndex ( index ) {}
  
    /// @group Relational Operators
  bool operator < ( basicBindingItem const& rhs ) const
    {
        // Optimization... change order to hint towards early return.
        // TODO: Optimization... skip things that "don't matter"
      SCENEDATALTCOMPAREHELPER(mSemanticType, rhs.mSemanticType);
      SCENEDATALTCOMPAREHELPER(mIndex, rhs.mIndex);
//      SCENEDATALTCOMPAREHELPER(mDataType, rhs.mDataType);
//      SCENEDATALTCOMPAREHELPER(mSize, rhs.mSize);
//      SCENEDATALTCOMPAREHELPER(mCount, rhs.mCount);
//      SCENEDATALTCOMPAREHELPER(mName, rhs.mName);
      return false;
    }
  
  bool operator == ( basicBindingItem const& rhs ) const
    {
      return (
          mSemanticType == rhs.mSemanticType &&
          mIndex == rhs.mIndex // &&
//          mDataType == rhs.mDataType &&
//          mSize == rhs.mSize &&
//          mCount == rhs.mCount &&
//          mName == rhs.mName
      );
    }
};

// ///////////////////////////////////////////////////////////////////

template< class BindingItem_ >
class basicBinding :
  public std::vector< BindingItem_ >
{
    using Base = std::vector< BindingItem_ >;
  public:
    using BindingItem = BindingItem_;
    using SemanticType = typename BindingItem::SemanticType;
    using DataType = typename BindingItem::DataType;
  
    size_t getValueStrideBytes () const
      {
        size_t ret = 0;
        for ( auto binding : *this )
          ret += binding.mSize * binding.mCount;
        return ret;
      }
  
      /// @warning Only useful if each Type can only be entered in the binding
      /// list at most one time.
    size_t getValueOffsetBytes ( SemanticType stype, size_t index = 0 ) const
      {
        size_t ret = 0;
        for ( auto binding : *this )
        {
          if ( binding.mSemanticType == stype && binding.mIndex == index )
            break;
          ret += binding.mSize * binding.mCount;
        }
        return ret;
      }

      /// @warning Only useful if a name can only be entered in the binding
      /// list at most one time.
    size_t getValueOffsetBytesByName ( std::string const& name, size_t index = 0 ) const
      {
        size_t ret = 0;
        for ( auto binding : *this )
        {
          if ( binding.mName == name && binding.mIndex == index )
            break;
          ret += binding.mSize * binding.mCount;
        }
        return ret;
      }

    bool hasBinding ( SemanticType stype, size_t index = 0 ) const
      {
        for ( auto binding : *this )
          if ( binding.mSemanticType == stype && binding.mIndex == index )
            return true;
        return false;
      }
  
    bool hasBindingByName ( std::string const& which, size_t index = 0 ) const
      {
        for ( auto binding : *this )
          if ( binding.mName == which && binding.mIndex == index )
            return true;
        return false;
      }

#ifdef SCENEDATABINDINGSARECACHED
      // Cached lookups seemed like a good optimzation, but it benched out ~40%
      // slower on x86.  Potentially, the pnipstd.h std::hash_combine could be
      // optimized to reduce arithmetic, but the simple iterations above are
      // likely always to be a win for bindings with < 5 entries.

      /// Set internal dirty state so that we can lazily eval internal caches.
    void setDirty ( bool val = true ) { mDirty = val; }

      // Hides base class version from std::vector... we need a side-effect when adding.
    void push_back ( typename Base::value_type const& val ) { setDirty (); Base::push_back ( val ); }
  
  private:

      // TRICKY: These maps are invalidated pretty much any time the bindings are
      // changed (due to vector realloc, or order changes).  That's why these
      // must be regenerated each time the bindings are changed.
  
    using OffsetMap = std::unordered_map< std::pair< SemanticType, size_t >, size_t >;
//    using ExistSet = std::unordered_set< std::pair< SemanticType, size_t > >;
  
    mutable OffsetMap mOffsets; // mutable for lazy updates
  
    void updateMaps () const
      {
        mOffsets.clear();
        size_t ret = 0;
        for ( auto binding : *this )
        {
          mOffsets[ std::make_pair( binding.mSemanticType, binding.mIndex ) ] = ret;
          ret += binding.mSize * binding.mCount;
        }
      }
  
    void clearDirty () const
      {
          if ( mDirty )
          {
            updateMaps();
            mDirty = false;
          }
      }
  
    mutable bool mDirty = true;
#endif // SCENEDATABINDINGSARECACHED
};

// ///////////////////////////////////////////////////////////////////

/**
  A class to manage values for packed, interleaved data, designed for
  things like vertex attributes, particle systems, uniforms, etc.
  Usage is 1) Add bindings to the #mBinding member, 2) #resize based on
  number of elements, 3) get pointers to values and stride through data
  as you fill in elements.
  @code
// Example usage of data, etc:

using dataBasic =        data<        basicBinding< basicBindingItem< BasicSemanticTypes, BasicDataTypes > > >;
using dataIndexedBasic = dataIndexed< basicBinding< basicBindingItem< BasicSemanticTypes, BasicDataTypes > > >;

  // see if we can use a std::string for the semantic type... yup... seems to work fine.
using dataIndexedString = dataIndexed< basicBinding< basicBindingItem< std::string, BasicDataTypes > > >;
  @endcode
*/

template< class Binding_, class LtCompare = lt_variadic<> >
class data :
  public LtCompare
{
  public:
    using ValueType = uint8_t;

    using Binding = Binding_;
    using BindingItem = typename Binding::BindingItem;
    using SemanticType = typename Binding::SemanticType;
    using DataType = typename Binding::DataType;

      // Public data!!!
      /// The binding object.  Use mBinding.push_back to add binding objects
      /// to this data object.
    Binding mBinding;

      /// Get the pointer to value storage for the specified binding and index.
      /// Return type is determined by the explicit invocation template type.
      /// @param element Which element to access.
      /// @param stype The semantic type of binding to access.
      /// @param index The index to access (if applicable, e.g., texture unit)
    template< typename Type >
    Type* getElementPtr ( size_t element, SemanticType const& stype, size_t index = 0 )
      {
          auto ptr = getPtr< ValueType > ();
          return ( reinterpret_cast< Type* > (
              ptr ?
              ptr + mBinding.getValueStrideBytes () * element + mBinding.getValueOffsetBytes ( stype, index ) : nullptr ) );
      }
  
    template< typename Type >
    Type const* getElementPtr ( size_t element, SemanticType const& stype, size_t index = 0 ) const
      {
          auto ptr = getPtr< ValueType > ();
          return ( reinterpret_cast< Type const* > (
              ptr ?
              ptr + mBinding.getValueStrideBytes () * element + mBinding.getValueOffsetBytes ( stype, index ) : nullptr ) );
      }
  
      /// Iterator Definitions
    template< class Ret >
    class iterator
    {
        static_assert(std::is_standard_layout<Ret>::value,"return type for iterator is not standard layout");
        static_assert(std::is_pointer<Ret>::value == false,"destination type should not be a pointer");
    
        ValueType* mPtr;
        size_t mStride;
      
        friend class data;

        iterator ( ValueType* ptr, size_t stride ) :
          mPtr ( ptr ), mStride( stride ) {}
      
      public:
      
        iterator ( iterator const& rhs ) = default;
        iterator& operator = ( iterator const& rhs ) = default;

        iterator operator + ( size_t count ) const { return iterator ( mPtr + count * mStride, mStride ); }
        iterator& operator += ( size_t count ) { mPtr += count * mStride; return *this; }
        iterator& operator ++ () { mPtr += mStride; return *this; }
        iterator operator - ( size_t count ) const { return iterator ( mPtr - count * mStride, mStride ); }
        iterator& operator -= ( size_t count ) { mPtr -= count * mStride; return *this; }
        iterator& operator -- () { mPtr -= mStride; return *this; }

        bool operator < ( iterator const& rhs ) const { return mPtr < rhs.mPtr; }
        bool operator == ( iterator const& rhs ) const { return mPtr == rhs.mPtr; }
        bool operator != ( iterator const& rhs ) const { return mPtr != rhs.mPtr; }
      
        Ret* operator-> () { return reinterpret_cast<Ret*>(mPtr); }
        Ret const* operator-> () const { return reinterpret_cast<Ret const*>(mPtr); }
        Ret& operator* () { return *reinterpret_cast<Ret*>(mPtr); }
        Ret const& operator* () const { return *reinterpret_cast<Ret const*>(mPtr); }
      
        bool good () const { return mPtr != nullptr; }
    };

    template< class Ret >
    class const_iterator
    {
        static_assert(std::is_standard_layout<Ret>::value,"return type for const_iterator is not standard layout");
        static_assert(std::is_pointer<Ret>::value == false,"destination type should not be a pointer");

        ValueType const* mPtr;
        size_t mStride;
      
        friend class data;

        const_iterator ( ValueType const* ptr, size_t stride ) :
          mPtr ( ptr ), mStride( stride ) {}
      
      public:
        using NonConstIter = iterator<Ret>;
      
        const_iterator ( const_iterator const& rhs ) = default;
        const_iterator ( NonConstIter const& rhs ) : const_iterator { rhs.mPtr, rhs.mStride } {}
        const_iterator& operator = ( const_iterator const& rhs ) = default;
        const_iterator& operator = ( NonConstIter const& rhs )
          { mPtr = rhs.mPtr; mStride = rhs.mStride; }

        const_iterator operator + ( size_t count ) const { return const_iterator ( mPtr + count * mStride, mStride ); }
        const_iterator& operator += ( size_t count ) { mPtr += count * mStride; return *this; }
        const_iterator& operator ++ () { mPtr += mStride; return *this; }
        const_iterator operator - ( size_t count ) const { return const_iterator ( mPtr - count * mStride, mStride ); }
        const_iterator& operator -= ( size_t count ) { mPtr -= count * mStride; return *this; }
        const_iterator& operator -- () { mPtr -= mStride; return *this; }

        bool operator < ( const_iterator const& rhs ) const { return mPtr < rhs.mPtr; }
        bool operator == ( const_iterator const& rhs ) const { return mPtr == rhs.mPtr; }
        bool operator != ( const_iterator const& rhs ) const { return mPtr != rhs.mPtr; }
      
        Ret const* operator-> () const { return reinterpret_cast<Ret const*>(mPtr); }
        Ret const& operator* () const { return *reinterpret_cast<Ret const*>(mPtr); }
      
          /// @return bool Indicates if iterator is 'good', which means its internal
          /// pointer is valid, but will also turn true for iterators after #end,
          /// so it's not a termination predecate.
        bool good () const { return mPtr != nullptr; }
    };
  
      /// @group Iterators Accessors
    template< class Ret >
    iterator< Ret > begin ( SemanticType stype, size_t index = 0 )
      {
        ValueType* ptr = getElementPtr < ValueType >( 0, stype, index );
        size_t stride = mBinding.getValueStrideBytes ();
        return iterator< Ret > ( ptr, stride );
      }

      /// Convenience to get iterator to first binding in element
    template< class Ret >
    iterator< Ret > begin ()
      {
        auto binding = mBinding[ 0 ];
        return begin ( binding->mSemanticType, binding->mIndex );
      }

    template< class Ret >
    iterator< Ret > end ( SemanticType stype, size_t index = 0 )
      {
        iterator< Ret > tmp = begin< Ret > ( stype, index );
        tmp += size ();
        return tmp;
      }

      /// Convenience to get iterator to first binding in element
    template< class Ret >
    iterator< Ret > end ()
      {
        auto binding = mBinding[ 0 ];
        return end ( binding->mSemanticType, binding->mIndex );
      }

    template< class Ret >
    const_iterator< Ret > begin ( SemanticType stype, size_t index = 0 ) const
      {
        ValueType const* ptr = getElementPtr < ValueType const >( 0, stype, index );
        size_t stride = mBinding.getValueStrideBytes ();
        return const_iterator< Ret > ( ptr, stride );
      }

      /// Convenience to get iterator to first binding in element
    template< class Ret >
    const_iterator< Ret > begin () const
      {
        auto binding = mBinding[ 0 ];
        return begin ( binding->mSemanticType, binding->mIndex );
      }

    template< class Ret >
    const_iterator< Ret > end ( SemanticType stype, size_t index = 0 ) const
      {
        const_iterator< Ret > tmp = begin< Ret > ( stype, index );
        tmp += size ();
        return tmp;
      }

      /// Convenience to get iterator to first binding in element
    template< class Ret >
    const_iterator< Ret > end () const
      {
        auto binding = mBinding[ 0 ];
        return end ( binding->mSemanticType, binding->mIndex );
      }

      /// Resize storage for elements, reflecting current binding settings.
      /// @param elements The new number of elements... NOT the byte size.
      /// @warning Invalidates any iterators.
    void resize ( size_t elements ) { mValues.resize ( mBinding.getValueStrideBytes () * elements ); }
      /// Get number of elements (not values, characters or bytes) in values.  Elements
      /// are the aggregate type stored in the values array.  E.g., all values for a
      /// single vertex.
    size_t size () const { return mValues.size() / mBinding.getValueStrideBytes (); }
      /// Get size in bytes of values storage.
    size_t sizeBytes () const { return mValues.size (); }
  
      /// Shrink the internal storage to exactly fit the current size.
    void shrink () { mValues.shrink_to_fit(); }
  
      /// Swap storage members to rhs a la STL collection swap.
    void swap ( data& rhs ) { mBinding.swap ( rhs.mBinding ); mValues.swap ( rhs.mValues ); }
  
      /// Change the layout of the packed elements.  Maintain values in
      /// bindings that aren't removed and create new space in each element
      /// for new bindings.
    void migrate ( Binding const& rhs );

      // I don't think we need these in public... the getElementPtr functions are as
      // general as we need, and the base pointer isn't all that useful by itself.
    template< typename Type >
    Type* getPtr () { return ( mValues.empty() ? nullptr : reinterpret_cast<Type*>(mValues.data()) ); }
    template< typename Type >
    Type const* getPtr () const { return ( mValues.empty() ? nullptr : reinterpret_cast<Type const*>(mValues.data()) ); }

      /// Less-than relational function for pointers into data value array.
      /// Will iterate through heterogeneous data types of arbitrary lengths
      /// as directed by bindings.
    bool lt ( ValueType const* lhs, ValueType const* rhs ) const
    {
      LtCompare const& ltCompare = *this;
      for ( auto cur : mBinding )
      {
        bool lhsltrhs = ltCompare( cur.mCount, cur.mDataType, lhs, rhs );
          // lhs < rhs
        if ( lhsltrhs ) return true;
        
        bool rhsltlhs = ltCompare( cur.mCount, cur.mDataType, rhs, lhs );
          // lhs > rhs
        if ( rhsltlhs ) return false;

          // lhs == rhs, continue
        size_t incr = cur.mSize * cur.mCount;
        lhs += incr;
        rhs += incr;
      }
    
      return false;
    }

  protected:

  
  private:
  
    using Values = std::vector< ValueType >;
    Values mValues;
};

// ///////////////////////////////////////////////////////////////////
/**
  A class to manage values for packed, interleaved data.  Building
  on the #data class, adds a vector of indices of the specified type
  to facilitate things like indexing for GLES glDrawElements, for a
  line rendering primitive length list, etc.
*/
template< class Binding, class LtCompare = lt_variadic<>, typename IndexType_ = uint16_t >
class dataIndexed :
  public data< Binding, LtCompare >
{
    using Base = data< Binding, LtCompare >;
  
  public:
    using IndexType = IndexType_;
    using Indices = std::vector< IndexType >;
  
    Indices& getIndices () { return mIndices; }
    Indices const& getIndices () const { return mIndices; }
    IndexType* getIndicesPtr () { return mIndices.data (); }
    IndexType const* getIndicesPtr () const { return mIndices.data (); }

    size_t sizeBytesIndices () const { return mIndices.size () * sizeof ( IndexType ); }
  
    void resize ( size_t elements ) = delete;
      /// Resize storage for both elements and indices, reflecting current
      /// binding settings.
      /// @warning Invalidates any iterators.
    void resize ( size_t elements, size_t indices ) { Base::resize ( elements ); mIndices.resize ( indices ); }
  
    void swap ( Base& rhs ) = delete;
    void swap ( dataIndexed& rhs ) { Base::swap ( rhs ); mIndices.swap ( rhs.mIndices ); }
  
  private:
  
    Indices mIndices;
};

// ///////////////////////////////////////////////////////////////////

// Example usage of data, etc:

using dataBasic =        data<        basicBinding< basicBindingItem< BasicSemanticTypes, BasicDataTypes > >, BasicLtCompare >;
using dataIndexedBasic = dataIndexed< basicBinding< basicBindingItem< BasicSemanticTypes, BasicDataTypes > >, BasicLtCompare >;

  // see if we can use a std::string for the semantic type... yup... seems to work fine.
using dataIndexedString = dataIndexed< basicBinding< basicBindingItem< std::string, BasicDataTypes > >, BasicLtCompare >;


// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// Out of line definitions to keep declarations tidy.

// TODO: Add another template parameter for concrete lt_variadic so that
// derived classes only have to implement a list of lt_pairs to generate
// less than functions for interleaved data elements (e.g., verts) of any
// heterogeneous types.

template< class Binding, class LtCompare >
void  data< Binding, LtCompare >::migrate ( Binding const& rhs )
{
  assert ( ! mBinding.empty () );
  assert ( ! rhs.empty () );
  
  if ( mBinding == rhs )
    return;
  
  size_t const elements = size ();  // not byte size
  size_t const lastElement = elements - 1;
  
    // Tricky... resize storage to _new_ binding size.
  mValues.resize ( rhs.getValueStrideBytes () * elements );
  
  size_t const srcStride = mBinding.getValueStrideBytes ();
  size_t const dstStride = rhs.getValueStrideBytes ();
  bool const growing = mBinding.getValueStrideBytes () < rhs.getValueStrideBytes ();
  
  for ( auto const& binding : mBinding )
  {
    if ( rhs.hasBinding ( binding.mSemanticType, binding.mIndex ) )
    {
      size_t const srcOffset = mBinding.getValueOffsetBytes ( binding.mSemanticType, binding.mIndex);
      size_t const dstOffset = rhs.getValueOffsetBytes ( binding.mSemanticType, binding.mIndex);
      size_t const componentSize = binding.mSize * binding.mCount;
      
      if ( growing )
      {
        ValueType* src = getPtr< ValueType >() + srcOffset + srcStride * lastElement;
        ValueType* dst = getPtr< ValueType >() + dstOffset + dstStride * lastElement;

        for ( size_t num = 0; num < elements; ++num, src -= srcStride, dst -= dstStride )
          memcpy ( dst, src, componentSize );
      }
      else
      {
        ValueType* src = getPtr< ValueType >() + srcOffset;
        ValueType* dst = getPtr< ValueType >() + srcOffset;

        for ( size_t num = 0; num < elements; ++num, src += srcStride, dst += dstStride )
          memcpy ( dst, src, componentSize );
      }
    }
  }
  mBinding = rhs;
}


// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenedata_h
