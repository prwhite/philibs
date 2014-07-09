// ///////////////////////////////////////////////////////////////////
//
//  class: dirtyProp
//
// ///////////////////////////////////////////////////////////////////

#ifndef scenedirty_h
#define scenedirty_h

// ///////////////////////////////////////////////////////////////////

#include "pniautoref.h"
#include "pnitypetraits.h"

// ///////////////////////////////////////////////////////////////////

namespace scene {
    
// ///////////////////////////////////////////////////////////////////

/**
  Base class for any class that wants to have cononical "dirty" behavior
  with the ability to override that behavior.
  
  Can be used with #dirtyProp, but dirtyProp can also work with any class
  that simply has the proper signatures for setDirty, clearDirty, and getDirty.
  
  @note clearDirty is @p const because it is expected to be called during
  evaluation/query of a const object, allowing the "dirty" state to be cleaned
  up lazily.
*/

class dirtyItem
{
  public:
    virtual void setDirty ( bool val = true ) {}
    virtual void clearDirty () const { mDirty = false; }
    virtual bool getDirty () const { return mDirty; }
  
  private:
    mutable bool mDirty = true;
};

// ///////////////////////////////////////////////////////////////////

namespace dirtyPropImpl
{

GENERATE_HAS_MEMBER(setDirty);

/**
  dirtyInvoker is an internal class that figures out whether a dirtyProp
  referenced class has the interface of #dirtyItem (either directly or
  by convention).
  
  If the referenced class does have the #dirtyItem interface, it will be
  invoked through setDirty, etc.  If not, those functions essentially no-op.
*/

template< typename Type, typename Enable = void >
struct dirtyInvoker
{
  static void setDirty_ ( Type& obj, bool val = true ) {}
  static void clearDirty_ ( Type const& obj ) {}
  static bool getDirty_ ( Type const& obj ) { return false; }
  
  static uint8_t const dbg = 0;
};

template< typename Type >
struct dirtyInvoker< Type, typename std::enable_if< has_member_setDirty< Type >::value >::type >
{
  static void setDirty_ ( Type& obj, bool val = true ) { obj.setDirty ( val ); }
  static void clearDirty_ ( Type const& obj ) { obj.clearDirty (); }
  static bool getDirty_ ( Type const& obj ) { return obj.getDirty (); }

  static uint8_t const dbg = 1;
};

template< typename Type >
struct dirtyInvoker< Type*, typename std::enable_if< has_member_setDirty< Type >::value >::type >
{
  static void setDirty_ ( Type* obj, bool val = true ) { if ( obj ) obj->setDirty ( val ); }
  static void clearDirty_ ( Type const* obj ) { if ( obj ) obj->clearDirty (); }
  static bool getDirty_ ( Type const* obj ) { return obj ? obj->getDirty () : false; }

  static uint8_t const dbg = 2;
};

template< typename Type >
struct dirtyInvoker< pni::pstd::autoRef< Type >, typename std::enable_if< has_member_setDirty< Type >::value >::type >
{
  using RefType = pni::pstd::autoRef< Type >;

  static void setDirty_ ( RefType& obj, bool val = true ) { if ( obj.get() ) obj->setDirty ( val ); }
  static void clearDirty_ ( RefType const& obj ) { if ( obj.get() ) obj->clearDirty (); }
  static bool getDirty_ ( RefType const& obj ) { return obj.get() ? obj->getDirty () : false; }

  static uint8_t const dbg = 3;
};

namespace // anonymous
{
    // Let's make sure the basics of the specilaizations are working...

  struct notDirty {};
  using notDirtyItemInvokerTest = dirtyInvoker< notDirty >;
  static_assert( notDirtyItemInvokerTest::dbg == 0, "wrong invoker specialization for notDirtyItem" );

  using dirtyItemInvokerTest = dirtyInvoker< dirtyItem >;
  static_assert( dirtyItemInvokerTest::dbg == 1, "wrong invoker specialization for dirtyItem" );
  
    // TODO: checks for pointer and autoRef types.
}

// ///////////////////////////////////////////////////////////////////

  // See http://www.parashift.com/c++-faq/nondependent-name-lookup-members.html
  // for why this-> is used in the derived classes below.

template< typename Type, class ThisType >
class dirtyPropBase
{
    using SetDirtyFuncType = void ( ThisType::* ) ();
    using ClearDirtyFuncType = void ( ThisType::* ) ();

  public:

    using Invoker = dirtyInvoker< Type >;

    dirtyPropBase () = default;
    dirtyPropBase (
        Type const& val,
        SetDirtyFuncType setFunc,
        ClearDirtyFuncType clearFunc ) :
      mSetDirtyFunc { setFunc },
      mClearDirtyFunc { clearFunc },
      mVal { val }
      {}
    dirtyPropBase ( Type const& val ) :
        dirtyPropBase { val, nullptr, nullptr } {}
    dirtyPropBase ( ClearDirtyFuncType clearFunc ) :
        dirtyPropBase { {}, nullptr, clearFunc } {}
    dirtyPropBase ( Type const& val, ClearDirtyFuncType clearFunc ) :
        dirtyPropBase { val, nullptr, clearFunc } {}
  
  public:
    void setDirty ( bool val = true )
      {
        if ( ( ! mDirty ) && mSetDirtyFunc )
          ( ( ( ThisType* ) this )->*mSetDirtyFunc )();
        mDirty = val;
      }
    dirtyPropBase const& clearDirty () const
      {
        if ( ! mInClearDirty )
        {
          mInClearDirty = true;
          if ( mDirty && mClearDirtyFunc )
            ( ( ( ThisType* ) this )->*mClearDirtyFunc )();
          mDirty = false;
          mInClearDirty = false;
        }
        return *this;
      }
    bool getDirty () const { return mDirty; }
  
  protected:
    SetDirtyFuncType mSetDirtyFunc = nullptr;
    ClearDirtyFuncType mClearDirtyFunc = nullptr;
  
    Type mVal {};

  private:
    mutable bool mDirty = true;
    mutable bool mInClearDirty = false;
};

} // end namespace dirtyPropImpl

// ///////////////////////////////////////////////////////////////////

/**
  @classdesign
  This class manages a data member with an associated dirty flag.  It invalidates
  the flag for any mutating operation (#set, #operator =, #op).  The invalidated
  state will be cleaned up when #clearDirty is explicitly called, typically in a
  lazy fashion in the owning class.  The function object provided at construction
  will be called back any time clearDirty determines state needs to be updated,
  otherwise it no-ops.  Additionally, a #setDirty callback can be specified to
  further allow the owning instance to customize behavior.  As an example,
  use the #setFunc to invalidate bounds when setting the #geomeData on a #geom
  instance.  This clas is intended to use and work with default copy constructors,
  as long as the value member is safe for copy construction.
 
  This class acts like a smart pointer, but all of the usual deref ops are
  const.  To mutate a la operator->, use #op, which returns a ref/ptr as
  you would expect from non-const operator->.
 
  Finally, another way of understanding this class is that it is a property with
  lazy update behavior.  The class that contains an instance can specify the 
  hooks for set dirty and clear dirty behavior.
  
  @note The clearDirty method is only called explicitly by external code. Thus,
  this class updates its state only when that is called, not as a side effect
  of calling one of the dereference methods.  E.g., #set, #operator==,
  and #operator-> do not call #clearDirty.
  
  @note This class is intended to be used in a CRTP
  http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
  situation, only because it needs to know @p this in order to properly
  invoke the member functions for set and clear callbacks.  Doing this through
  aggregation leaves this class without any proper sense of @p this... which can
  be dealt with by capturing @p this in a lambda, but that causes @p this to
  be incorrectly referenced when making copies of the owning object.
  
  To use this class, you will generally do the following:
 
  1) Create a typedef @a somewhere that makes things a little easier. Ex:
  @code
    class foo;  // fwd declare
    using DirtyInt = scene::dirtyProp< int, foo >;
  @endcode
  2) Inherit your target class from this class. Ex:
  @code
    class foo :
      public DirtyInt
      ...
  @endcode
  3a) Initialize the dirty base class in the constructor (both callbacks are
  optional and can be left as @p nullptr):
  @code
      foo () : DirtyInt { 0, &foo::setIntDirtyFunc, &foo::clearIntDirtyFunc }
      ...
  @endcode
  3b) Create the callbacks for when a property is set or needs to be lazily
  updated. Ex:
  @code
      void setIntDirtyFunc ()
        {
          // Do any additional state invalidation
        }
      void clearIntDirtyFunc ()
        {
          // Update dirty state as needed
        }
  @endcode
  4) Optional: Create some accessor functions to avoid unsightly invocation syntax... or not. Ex:
  @code
      DirtyInt& dirtyInt () { return *this; }
      DirtyInt const& dirtyInt () const { return *this; }
  @endcode
  Client code then looks like this:
  @code
    pFoo->dirtyInt().op() = 3;
  @endcode
  ... rather than this:
  @code
    pFoo->DirtyInt::op() = 3;
  @endcode
  This optional syntax is also better when using @p operator* and @p operator->.
*/

template< class Type, class ThisType, size_t uniquifier = 0 >
class dirtyProp :
  public dirtyPropImpl::dirtyPropBase< Type, ThisType >
{
  public:
    using dirtyPropImpl::dirtyPropBase<Type, ThisType>::dirtyPropBase;
  
    dirtyProp& operator = ( Type const& val ) { this->setDirty(); this->mVal = val; return *this; }
    void set ( Type const& val ) { *this = val; }   // delegates to operator =
    Type const& get () const { return this->mVal; }
    Type& op () { this->setDirty (); return this->mVal; }
    Type const* operator -> () const { return &this->mVal; }
    bool operator == ( dirtyProp const& rhs ) { return this->mVal == rhs.mVal; }
};

  // Specialization for pni::pstd::autoRef
template< class Type, class ThisType, size_t uniquifier >
class dirtyProp< pni::pstd::autoRef< Type >, ThisType, uniquifier > :
  public dirtyPropImpl::dirtyPropBase< pni::pstd::autoRef< Type >, ThisType >
{
  public:
    using dirtyPropImpl::dirtyPropBase< pni::pstd::autoRef< Type >, ThisType >::dirtyPropBase;

    using ref_type = pni::pstd::autoRef< Type >;
    using value_type = typename ref_type::value_type;

    dirtyProp& operator = ( value_type* val ) { this->setDirty(); this->mVal = val; return *this; }
    void set ( value_type* pVal ) { *this = pVal; } // delegates to operator =
    value_type* get () { return this->mVal.get(); } // need non-const... careful with it though
    value_type const* get () const { return this->mVal.get(); }
    value_type* op () { this->setDirty (); return this->mVal.get(); }
    operator bool () const { return static_cast< bool > ( this->mVal ); }
    ref_type const& operator -> () const { return this->mVal; }
//    value_type const& operator* () const { return *this->mVal.get(); }
    bool operator == ( dirtyProp const& rhs ) { return this->mVal == rhs.mVal; }
};



// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenedirty_h
