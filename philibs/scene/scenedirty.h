// ///////////////////////////////////////////////////////////////////
//
//  class: dirty
//
// ///////////////////////////////////////////////////////////////////

#ifndef scenedirty_h
#define scenedirty_h

// ///////////////////////////////////////////////////////////////////

#include "pniautoref.h"

// ///////////////////////////////////////////////////////////////////

namespace scene {
    
// ///////////////////////////////////////////////////////////////////


  // See http://www.parashift.com/c++-faq/nondependent-name-lookup-members.html
  // for why this-> is used in the derived classes below.

template< typename Type >
class dirtyBase
{
  public:
    dirtyBase () = default;
    dirtyBase (
        Type const& val,
        std::function< void () > const& setFunc,
        std::function< void () > const& clearFunc ) :
      mSetDirtyFunc { setFunc },
      mClearDirtyFunc { clearFunc },
      mVal { val }
      {}
    dirtyBase ( Type const& val ) :
        dirtyBase { val, [](){}, [](){} } {}
    dirtyBase ( std::function< void () > const& clearFunc ) :
        dirtyBase { {},  [](){}, clearFunc } {}
    dirtyBase ( Type const& val, std::function< void () > const& clearFunc ) :
        dirtyBase { val,  [](){}, clearFunc } {}
  
  public:
    void setDirty ( bool val = true ) { mSetDirtyFunc(); mDirty = val; }
    bool getDirty () const { return mDirty; }
    void clearDirty () const { if ( mDirty ) mClearDirtyFunc(); mDirty = false; }
  
  protected:
    std::function<void()> mSetDirtyFunc = [](){};
    std::function<void()> mClearDirtyFunc = [](){};
    Type mVal {};

  private:
    mutable bool mDirty = true;
};

/**
  This class manages a data member with an associated dirty flag.  It invalidates
  the flag for any mutating operation (#set, #operator =, #op).  The invalidated
  state will be cleaned up when #clearDirty is explicitly called, typically in a
  lazy fashion in the owning class.  The function object provided at construction
  will be called back any time clearDirty determines state needs to be updated,
  otherwise it no-ops.  Additionally, a #setDirty callback can be specified to
  further allow the owning instance to customize behavior.  As an example,
  use the #setFunc to invalidate bounds when setting the #geomeData on a #geom
  instance.
 
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
*/

template< class Type, size_t uniquifier = 0 >
class dirty :
  public dirtyBase< Type >
{
  public:
    using dirtyBase<Type>::dirtyBase;
  
    dirty& operator = ( Type const& val ) { this->setDirty(); this->mVal = val; return *this; }
    void set ( Type const& val ) { *this = val; }   // delegates to operator =
    Type const& get () const { return this->mVal; }
    Type& op () { this->setDirty (); return this->mVal; }
//    operator bool () const { return static_cast< bool > ( this->mVal ); } // Doesn't make sense for non-specialized version
    Type* operator -> () const { return &this->mVal; }
    bool operator == ( dirty const& rhs ) { return this->mVal == rhs.mVal; }
};

  // Specialization for pni::pstd::autoRef
template< class Type, size_t uniquifier >
class dirty< pni::pstd::autoRef< Type >, uniquifier > :
  public dirtyBase< pni::pstd::autoRef< Type > >
{
  public:
    using dirtyBase< pni::pstd::autoRef< Type > >::dirtyBase;

    using ref_type = pni::pstd::autoRef< Type >;
    using value_type = typename ref_type::value_type;

    dirty& operator = ( value_type* val ) { this->setDirty(); this->mVal = val; return *this; }
    void set ( value_type* pVal ) { *this = pVal; } // delegates to operator =
    value_type const* get () const { return this->mVal.get(); }
    value_type* op () { this->setDirty (); return this->mVal.get(); }
    operator bool () const { return static_cast< bool > ( this->mVal ); }
    ref_type const& operator -> () const { return this->mVal; }
    bool operator == ( dirty const& rhs ) { return this->mVal == rhs.mVal; }
};



// ///////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenedirty_h