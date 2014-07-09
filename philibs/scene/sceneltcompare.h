// ///////////////////////////////////////////////////////////////////
//
//  class: ltcompare
//
// ///////////////////////////////////////////////////////////////////

#ifndef sceneltcompare_h
#define sceneltcompare_h

// ///////////////////////////////////////////////////////////////////

#include <iostream>
#include <type_traits>

// ///////////////////////////////////////////////////////////////////

namespace scene {
    
// ///////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////
// All of the lt_* classes create structure for the < relational test
// between heterogeneous blocks of memory contained by a data instance.

  /// Type used for tying an type id enumeration value and an associated
  /// actual static type.  For use with #lt_variadic class.
template< int IdVal, typename DataType_ >
struct lt_pair
{
  using DataType = DataType_;
  constexpr static int idVal () { return IdVal; }
};

  /// type traits for lt_pair
  /// base trait says 'no'
template< class PairType >
struct is_lt_pair
{
  static bool const value = false;
};

  /// specialization for lt_pair says 'yes'
template< int IdVal, class DataType >
struct is_lt_pair< lt_pair< IdVal, DataType > >
{
  static bool const value = true;
};

// ///////////////////////////////////////////////////////////////////

// http://stackoverflow.com/questions/16991853/c-varadic-template-class-termination

  /// Base template for generating lt compare of blocks of heterogeneous data...
  /// e.g., arrays of vertex data of varying types.
template< typename... >
struct lt_variadic;

  /// Base template for comparing n consecutive homogeneous values.  Called by
  /// op () of lt_variadic once it has decided it has the right type.
template< class LtVariadic, class Type, class Enable = void >
struct lt_n;

  /// Base template for streaming output of n consecutive homogeous values.
  /// Called by lt_variadic ostream method once it has decided it has the right
  /// type.
template< class Type >
struct lt_ostream_n;

  /// Implementation of main iteration over valid data types that can be
  /// lt compared (function call operator).  Also implementation of n-way
  /// comparator for values once data type is determined (lt_n).
template< typename PairType, typename... Args >
struct lt_variadic< PairType, Args... >
{
    using ThisType = lt_variadic< PairType, Args... >;

    static_assert(is_lt_pair<PairType>::value, "PairType does not appear to honor contract with lt_variadic");

      /// Implementation of lt compare for @p num items of contiguous memory of
      /// given @p DataType.
      /// @param num The number of consecutive elements to compare
      /// @param idVal The enumerant for the desired data type
      /// @param lhs The left hand side memory pointer, to be cast to DataType
      /// @param rhs The right hand side memory pointer, to be cast to DataType
    using ValueType = uint8_t;
    bool operator () ( size_t num, size_t idVal, ValueType const* lhs, ValueType const* rhs ) const
    {
      using DataType = typename PairType::DataType;
      if ( idVal == PairType::idVal() )
        return lt_n< ThisType, DataType >()( this, ( DataType const* ) lhs, ( DataType const* ) rhs, num );
      else
        return lt_variadic< Args... >() ( num, idVal, lhs, rhs );
    }
  
    /// @group Epsilon value management.
  private:
    using EpsilonType = uint64_t;
    EpsilonType mEpsilon = 0;  // Just to get 8 bytes of storage, for float or double

  public:
    template< class Type >
    void setEpsilon ( Type val )
        {
          static_assert ( sizeof(Type) <= sizeof(EpsilonType), "Type too wide for storage" );
          * ( Type* ) ( &mEpsilon ) = val ;
        }
    template< class Type >
    Type getEpsilon () const
        {
          static_assert ( sizeof(Type) <= sizeof(EpsilonType), "Type too wide for storage" );
          return * ( Type* ) ( &mEpsilon );
        }
  
  public:
    void ostream ( std::ostream& ostr, size_t num, size_t idVal, ValueType const* vals ) const
      {
        using DataType = typename PairType::DataType;
        if ( idVal == PairType::idVal() )
          return lt_ostream_n< DataType >()( ostr, (DataType*) vals, num );
        else
          return lt_variadic< Args... >().ostream ( ostr, num, idVal, vals );
      }
  
};

  /// Compare @p num elements of a given data type, producing relational
  /// less than result.
template< class LtVariadic, class Type >
struct lt_n< LtVariadic, Type, typename std::enable_if< ! std::is_floating_point<Type>::value >::type >
{
  bool operator () ( LtVariadic const* pVariadic, Type const* lhs, Type const* rhs, size_t num ) const
  {
    switch ( num )
    {
      case 1: return *lhs < *rhs;
      case 2: return std::tie(lhs[0],lhs[1]) < std::tie(rhs[0],rhs[1]);
      case 3: return std::tie(lhs[0],lhs[1],lhs[2]) < std::tie(rhs[0],rhs[1],rhs[2]);
      case 4: return std::tie(lhs[0],lhs[1],lhs[2],lhs[3]) < std::tie(rhs[0],rhs[1],rhs[2],rhs[3]);
      case 0:
      default:
        return false;
    }
  }
};

  /// Compare @p num elements of a given data type, producing relational
  /// less than result, with epsilon compare.
  /// @note The epsilon compare is biased toward giving an equal result
  /// (widening around @p this, rather than biased toward giving more less-than
  /// results).
  /// @see http://en.cppreference.com/w/cpp/types/enable_if
  /// TODO: Need to add half float to is_floating_point or equivalent
template< class LtVariadic, class Type >
struct lt_n< LtVariadic, Type, typename std::enable_if< std::is_floating_point<Type>::value >::type >
{
  bool operator () ( LtVariadic const* pVariadic, Type const* lhs, Type const* rhs, size_t num ) const
  {
    Type eps = pVariadic->template getEpsilon<Type>();
    switch ( num )
    {
        // forward_as_tuple because we have an rvalue due to the arithmetic.
      case 1: return lhs[0] < rhs[0]-eps;
      case 2: return std::tie(lhs[0],lhs[1]) < std::forward_as_tuple(rhs[0]-eps,rhs[1]-eps);
      case 3: return std::tie(lhs[0],lhs[1],lhs[2]) < std::forward_as_tuple(rhs[0]-eps,rhs[1]-eps,rhs[2]-eps);
      case 4: return std::tie(lhs[0],lhs[1],lhs[2],lhs[3]) < std::forward_as_tuple(rhs[0]-eps,rhs[1]-eps,rhs[2]-eps,rhs[3]-eps);
      case 0:
      default:
        return false;
    }
  }
};


  /// Ran out of legitimate data types to try, so always false, which means
  /// unknown types will still allow equality for the other "members".
template<>
struct lt_variadic<>
{
  using ValueType = uint8_t;
  bool operator () ( size_t num, size_t idVal, ValueType const* lhs, ValueType const* rhs )
  {
    return false;
  }
  void ostream ( std::ostream& ostr, size_t num, size_t idVal, ValueType const* vals ) const {}
};

template< class Type >
struct lt_ostream_n
{
  using ValueType = uint8_t;
  void operator () ( std::ostream& ostr, Type const* vals, size_t num )
    {
      for ( int cur = 0; cur < num; ++cur )
      {
        ostr << vals[ cur ];
        if ( cur != num - 1)
          ostr << ", ";
      }
    }
};

// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // sceneltcompare_h
