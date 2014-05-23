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

// ///////////////////////////////////////////////////////////////////

namespace scene {

// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
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
  
  type_float_16_t,
  type_float_32_t,
  type_float_64_t,

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
  
    /// @group Operators
  bool operator < ( basicBindingItem const& rhs ) const
    {
        // Optimization... change order to hint towards early return.
        // TODO: Optimization... skip things that "don't matter"
      SCENEDATALTCOMPAREHELPER(mSemanticType, rhs.mSemanticType);
      SCENEDATALTCOMPAREHELPER(mIndex, rhs.mIndex);
      SCENEDATALTCOMPAREHELPER(mDataType, rhs.mDataType);
      SCENEDATALTCOMPAREHELPER(mSize, rhs.mSize);
      SCENEDATALTCOMPAREHELPER(mCount, rhs.mCount);
      SCENEDATALTCOMPAREHELPER(mName, rhs.mName);
      return false;
    }
  
  bool operator == ( basicBindingItem const& rhs ) const
    {
      return ( mName = rhs.mName &&
          mName = rhs.mName &&
          mSemanticType = rhs.mSemanticType &&
          mDataType = rhs.mDataType &&
          mSize = rhs.mSize &&
          mCount = rhs.mCount &&
          mIndex = rhs.mIndex );
    }
};

// ///////////////////////////////////////////////////////////////////

template< class BindingItem >
class basicBinding :
  public std::vector< BindingItem >
{
  public:
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
    size_t getValueOffsetBytes ( SemanticType which, size_t index ) const
      {
        size_t ret = 0;
        for ( auto binding : *this )
        {
          if ( binding.mSemanticType == which && binding.mIndex == index )
            break;
          ret += binding.mSize * binding.mCount;
        }
        return ret;
      }

      /// @warning Only useful if a name can only be entered in the binding
      /// list at most one time.
    size_t getValueOffsetBytes ( std::string const& name, size_t index ) const
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

    bool hasBinding ( SemanticType which, size_t index = 0 ) const
      {
        for ( auto binding : *this )
          if ( binding.mSemanticType == which && binding.mIndex == index )
            return true;
        return false;
      }
  
    bool hasBinding ( std::string const& which, size_t index = 0 ) const
      {
        for ( auto binding : *this )
          if ( binding.mName == which && binding.mIndex == index )
            return true;
        return false;
      }
  
  private:
  
      // TODO: Track "dirty" state to optimize #hasBinding queries.
};

// ///////////////////////////////////////////////////////////////////
/**
  A class to manage values for packed, interleaved data, designed for
  things like vertex attributes, particle systems, uniforms, etc.
  Usage is 1) Add bindings to the #mBinding member, 2) #resize based on
  number of elements, 3) get pointers to values and stride through data
  as you fill in elements.
*/

template< class Binding >
class data 
{
  public:

      // Public data!!!
      /// The binding object.  Use mBinding.push_back to add binding objects
      /// to this data object.
    Binding mBinding;

    template< typename Type >
    Type* getElementPtr ( size_t count, typename Binding::SemanticType which, size_t index = 0 )
      { return ( mValues.empty() ? 0 :
        reinterpret_cast<Type*>(mValues.data() + mBinding.getValueStrideBytes () * count + mBinding.getValueOffsetBytes ( which, index ) ) ); }
    template< typename Type >
    Type const* getElementPtr ( size_t count, typename Binding::SemanticType which, size_t index = 0 ) const
      { return ( mValues.empty() ? 0 :
        reinterpret_cast<Type*>(mValues.data() + mBinding.getValueStrideBytes () * count + mBinding.getValueOffsetBytes ( which, index ) ) ); }

      /// Resize the data with the given number of elements.
      /// @param elements The new number of elements... NOT the byte size.
    void resize ( size_t elements )
      {
        mValues.resize ( mBinding.getValueStrideBytes () * elements );
      }
      /// Get count of elements in values.
    size_t size () const { return mValues.size() / mBinding.getValueStrideBytes (); }
      /// Get size in bytes of values storage.
    size_t sizeBytes () const { return mValues.size (); }
  
      /// Shrink the internal storage to exactly fit the current size.
    void shrink () { mValues.shrink_to_fit(); }

  protected:

      // I don't think we need these... the getElementPtr functions are as
      // general as we need, and the base pointer isn't all that useful by itself.
    template< typename Type >
    Type* getPtr () { return ( mValues.empty() ? 0 : reinterpret_cast<Type*>(mValues.data()) ); }
    template< typename Type >
    Type const* getPtr () const { return ( mValues.empty() ? 0 : reinterpret_cast<Type*>(mValues.data()) ); }
  
  private:
  
    using ValueType = uint8_t;
    using Values = std::vector< ValueType >;
    Values mValues;
};

// ///////////////////////////////////////////////////////////////////
/**
  A class to manage values for packed, interleaved data.  Building
  on the #data class, adds a
*/
template< class Binding, typename IndexType_ = uint16_t >
class dataIndexed :
  public data< Binding >
{
    using Base = data< Binding >;
  
  public:
    using IndexType = IndexType_;
    using Indices = std::vector< IndexType >;
  
    Indices& getIndices () { return mIndices; }
    Indices const& getIndices () const { return mIndices; }
    IndexType* getIndicesPtr () { return mIndices.data (); }
    IndexType const* getIndicesPtr () const { return mIndices.data (); }
  
    void resize ( size_t elements ) = delete;
    void resize ( size_t elements, size_t indices )
      {
        Base::resize ( elements );
        mIndices.resize ( indices );
      }
  
  private:
  
    Indices mIndices;
};

// ///////////////////////////////////////////////////////////////////

using dataBasic =        data<        basicBinding< basicBindingItem< BasicSemanticTypes, BasicDataTypes > > >;
using dataIndexedBasic = dataIndexed< basicBinding< basicBindingItem< BasicSemanticTypes, BasicDataTypes > > >;

// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenedata_h
