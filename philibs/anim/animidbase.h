/////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////

#ifndef animidbase_h
#define animidbase_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "pniautoref.h"

  // For possible loss of data in reinterpret cast.
#pragma warning( push )
#pragma warning( disable : 4311 )

/////////////////////////////////////////////////////////////////////

namespace anim {

/////////////////////////////////////////////////////////////////////

class idBase
{
  public:
    idBase () : mId ( 0 ) {}
  
      // Id is used by anim::manager for uniqueness of animation targets.
      // So, if an anim group is added with an Id equal to an existing Id
      // the old one will be killed and the new one installed.  That is,
      // the manager uses a map of < id, group* > for all running animations.
    typedef size_t Id;
    //void setId ( Id id ) { mId = id; }
    
    template< class Type >
    void setId ( Type val ) { mId = ( Id ) ( val ); }
    
    Id getId () const { return mId; }
    
      // Manage anim target.  This always has to be something that
      // is refcounted, naturally.
      // The template return simplifies getting the abstract type
      // back to the specific target type.  If misused, things can
      // go very badly.
    //void setTarget ( pni::pstd::refCount* pTarget ) { mTarget = pTarget; }
    //
    //template< class Type >
    //Type* getTarget () const { return static_cast< Type* > ( mTarget.get () ); }

  protected:
    Id mId;
    
    //typedef pni::pstd::autoRef< pni::pstd::refCount > TargetRef;
    //TargetRef mTarget;
};

/////////////////////////////////////////////////////////////////////

} // end namespace anim

#pragma warning( pop )

/////////////////////////////////////////////////////////////////////

#endif // animidbase_h
