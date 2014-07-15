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
#include <cassert>

// ///////////////////////////////////////////////////////////////////

namespace scene {

// ///////////////////////////////////////////////////////////////////

namespace dirtyPropImpl
{

  // See http://www.parashift.com/c++-faq/nondependent-name-lookup-members.html
  // for why this-> is used in the derived classes below.

template< typename Type, class ThisType >
class dirtyPropBase
{
  protected:
    using SetDirtyFuncType = void ( ThisType::* ) ();
    using ClearDirtyFuncType = void ( ThisType::* ) () const;

  protected:

//    dirtyPropBase () = default;
    dirtyPropBase () = default;
    dirtyPropBase (
        SetDirtyFuncType setFunc,
        ClearDirtyFuncType clearFunc ) :
      mSetDirtyFunc { setFunc },
      mClearDirtyFunc { clearFunc }
      {}
//    dirtyPropBase ( Type const& val ) :
//        dirtyPropBase { val, nullptr, nullptr } {}
//    dirtyPropBase ( ClearDirtyFuncType clearFunc ) :
//        dirtyPropBase { {}, nullptr, clearFunc } {}
//    dirtyPropBase ( Type const& val, ClearDirtyFuncType clearFunc ) :
//        dirtyPropBase { val, nullptr, clearFunc } {}
  
  public:
    void setDirty ( bool val = true )
      {
        if ( val && ( ! mDirty ) && mSetDirtyFunc )
          ( static_cast< ThisType* > ( this )->*mSetDirtyFunc )();
        mDirty = val;
      }
    dirtyPropBase const& clearDirty () const
      {
        if ( ! mInClearDirty )
        {
          mInClearDirty = true;
          if ( mDirty && mClearDirtyFunc )
            ( static_cast< ThisType const* > ( this )->*mClearDirtyFunc )();
          mDirty = false;
          mInClearDirty = false;
        }
        else
        {
          assert(0);  // Should not be recursing in clearDirty.
        }
        return *this;
      }
    bool getDirty () const { return mDirty; }
  
  protected:
    SetDirtyFuncType mSetDirtyFunc = nullptr;
    ClearDirtyFuncType mClearDirtyFunc = nullptr;

  private:
    mutable bool mDirty = true;
    mutable bool mInClearDirty = false;
};

} // end namespace dirtyPropImpl

// ///////////////////////////////////////////////////////////////////

/**
  @classdesign
  This class manages a data member with an associated dirty flag.  It invalidates
  the flag for any mutating operation (currently only #op).  The invalidated
  state will be cleaned up when #clearDirty is explicitly called, typically in a
  lazy fashion in the owning class.  The function object provided at construction
  will be called back any time clearDirty determines state needs to be updated,
  otherwise it no-ops.  Additionally, a #setDirty callback can be specified to
  further allow the owning instance to customize behavior.  As an example,
  use the #setFunc to invalidate bounds when setting the #geomeData on a #geom
  instance.  This class is intended to use and work with default copy constructors,
  as long as the value member is safe for copy construction.
 
  This class acts like a smart pointer, but all of the usual deref ops are
  const.  To mutate a la operator->, use #op, which returns a ref/ptr as
  you would expect from non-const operator->.
 
  Finally, another way of understanding this class is that it is a property with
  lazy update behavior.  The class that contains an instance can specify the 
  hooks for set dirty and clear dirty behavior.
  
  @note The clearDirty method is only called explicitly by external code (e.g.,
  #clearDirty or #getUpdated). Thus,
  this class updates its state only when that is called, not as a side effect
  of calling one of the dereference methods.  E.g., #operator==,
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
  3a) Initialize the dirty base class in the constructor:
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
    using MemberPtr = Type ( ThisType::* );
    using BaseType = dirtyPropImpl::dirtyPropBase< Type, ThisType >;

    dirtyProp (
        MemberPtr ptr,
        typename BaseType::SetDirtyFuncType setFunc,
        typename BaseType::ClearDirtyFuncType clearFunc ) :
      BaseType ( setFunc, clearFunc ),
      mPtr ( ptr )
      {}

    Type& op () { BaseType::setDirty (); return ( static_cast< ThisType* >( this ) )->*mPtr; }
    Type const& get () const { return ( static_cast< ThisType const* >( this ) )->*mPtr; }
    Type const& getUpdated () const { BaseType::clearDirty (); return get (); }

    Type const* operator -> () const { return &get (); }
    Type const& operator * () const { return get (); }

  private:
    MemberPtr mPtr;
};

  // Specialization for when Type and ThisType are the same... the dirtyProp
  // will not contain a member pointer in this case, since it is clearly just
  // referring to itself.
template< class Type, size_t uniquifier >
class dirtyProp< Type, Type, uniquifier >  :
  public dirtyPropImpl::dirtyPropBase< Type, Type >
{
  public:
    using BaseType = dirtyPropImpl::dirtyPropBase< Type, Type >;

    dirtyProp () = default;
    dirtyProp (
        typename BaseType::SetDirtyFuncType setFunc,
        typename BaseType::ClearDirtyFuncType clearFunc ) :
      BaseType ( setFunc, clearFunc )
      {}

    Type& op () { BaseType::setDirty ( true ); return *static_cast< Type* > ( this ); }
    Type const& get () const { return *static_cast< Type const* > ( this ); }
    Type const& getUpdated () const { BaseType::clearDirty (); return get (); }

    Type const* operator -> () const { return &get (); }
    Type const& operator * () const { return get (); }

  private:
};

template< typename Type, typename ThisType, size_t uniquifier >
class dirtyProp< pni::pstd::autoRef< Type >, ThisType, uniquifier >
{
  // autoRef should not be used with dirtyProp... so this empty specialization
  // should cause nasty compilation errors.
  //
  // The reason is that
  // the 'dirty' value provided by dirtyProp should be tightly coupled
  // to the dirty item, e.g., anything derived from refCount which
  // generally denotes complex, heavyweight behavior.
  //
  // When the dirty value
  // and the implied dirty object are loosely coupled, it makes it hard
  // or impossible for downstream algorithms to 'clear dirty' when the
  // extrinsic dirty flag has not been passed into the algorithm as
  // well as the complex object being updated.
};

// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenedirty_h
