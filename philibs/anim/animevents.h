/////////////////////////////////////////////////////////////////////
//
//  
//
/////////////////////////////////////////////////////////////////////

#ifndef animevents_h
#define animevents_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "pniautoref.h"

#include "animbase.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
  class manager;
  
/////////////////////////////////////////////////////////////////////

class animEvent
{
  public:
    animEvent () :
      mManager ( 0 ),
      mBase ( 0 ),
      mTimeStamp ( 0.0f )
        {}
  
    void setTimeStamp ( TimeType val ) { mTimeStamp = val; }
    TimeType getTimeStamp () const { return mTimeStamp; }
    
    typedef pni::pstd::autoRef< base > BaseRef;
    void setBase ( base* pBase ) { mBase = pBase; }
    base* getBase () const { return mBase.get (); }
  
    manager* mManager;
    BaseRef mBase;
    TimeType mTimeStamp;
};

/////////////////////////////////////////////////////////////////////

} // end namespace anim

/////////////////////////////////////////////////////////////////////

#endif // animevents_h
