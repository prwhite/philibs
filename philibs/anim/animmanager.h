/////////////////////////////////////////////////////////////////////
//
//    class: manager
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimmanager_h
#define animanimmanager_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "pniautoref.h"
#include "pnibox.h"

#include "animgroup.h"
#include "animmatrixhelper.h"
#include "animeventobserver.h"

#include <map>
#include <unordered_map>
#include <vector>
#include <set>

/////////////////////////////////////////////////////////////////////

  // For possible loss of data in reinterpret cast.
#pragma warning( push )
#pragma warning( disable : 4311 )

/////////////////////////////////////////////////////////////////////

namespace anim {

class manager :
  public pni::pstd::refCount
{
  public:
    manager();
//     manager(manager const& rhs);
//     manager& operator=(manager const& rhs);
//     bool operator==(manager const& rhs) const;

    typedef group::Id Id;

      // Call every frame to evaluate animation.
    void update ( TimeType now );
    
      /////////
      // Add an animation to the manager.
      // TODO: After and Forever are not implemented.
      // UPDATE: Now and After are removed... to get the effect of
      // Now, just use 0.0f.  To get an offset from now, use a negative
      // number
      // Special values:
    static const TimeType Now;     // Start anim 'now' when submitted.
    //static const TimeType After;   // Start anim after any current anim on Id.
    static const TimeType Forever; // Never end animation... only to be used when looping.
    
    void addAnim ( group* pGroup );
    void remAnim ( group* pGroup );

      // Will clean up any unreffed matrixHelpers.
    void gcHelpers ();
    
    template< class Type >
    matrixHelper* allocHelper ( Type idIn )
        {
          Id id = ( Id ) idIn;
          
          if ( mHelpers[ id ].get () == 0 )
            mHelpers[ id ] = new matrixHelper;

          return mHelpers[ id ].get ();
        }


    void addFinalize ( base* pBase );

    float pow ( float val );
    
     // TODO add anim observers
     typedef std::set< animObserver* > Observers;
     void addAnimObserver ( animObserver* pObs );
     void remAnimObserver ( animObserver* pObs );
     
      // Some helper functions that make using the animation system
      // easier.
     void addTimerNotification ( TimeType when, animObserver* pObs, idBase::Id id = 0 );
     void addSimpleAnim ( TimeType when, base* pAnim, animObserver* pObs, idBase::Id id );
     
     void cancelObserver ( animObserver* pObs );
     
      // For debugging... only works when proper preproc directive
      // is enabled.  Right now only works on groups, not on bases
      // contained by groups.
    void setTrackBase ( base* pBase ) { mTrackBase = pBase; }
     
  protected:
    virtual ~manager();
    
    void execFinalizeList ();
    
    bool isStarting ( base* pBase, TimeType now );
    
    void doRemEvent ( group* pGroup, bool andRemove = false );
    
      // Used for debugging.
    void onTrackBaseAdd ( base* pBase );
    void onTrackBaseRem ( base* pBase );
    void onTrackBaseStart ( base* pBase );
    void onTrackBaseStartNegTime ( base* pBase );
    void onTrackBaseEnd ( base* pBase );
    void onTrackBaseFinalize ( base* pBase );
    void onTrackBaseReplace ( base* pBase );
    void onTrackBase ( base* pBase );
    
    template< class Event, class Func >
    void emitEvent ( base* pBase, Event const& anEvent, Func func );

  private:

    typedef pni::pstd::autoRef< group > GroupRef;
    typedef std::unordered_map< Id, GroupRef > Groups;
    
    Groups mGroups;
    
    typedef pni::pstd::autoRef< base > BaseRef;
//     typedef base* BaseRef;
    typedef std::unordered_map< Id, BaseRef > Bases;
    
    Bases mFinalizeList;
    
    typedef pni::pstd::autoRef< matrixHelper > HelperRef;
    typedef std::unordered_map
    < Id, HelperRef  > Helpers;
    
    Helpers mHelpers;
    
    Observers mObservers;
    
    TimeType mLastUpdate;
    
    pni::pstd::box< base* > mTrackBase;

  // interface from pni::pstd::refCount
  public:
    
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );

};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

/////////////////////////////////////////////////////////////////////

#pragma warning( pop )

/////////////////////////////////////////////////////////////////////

#endif // animanimmanager_h


