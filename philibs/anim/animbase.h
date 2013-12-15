/////////////////////////////////////////////////////////////////////
//
//    class: base
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimbase_h
#define animanimbase_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "pnimath.h"

#include "animidbase.h"
#include "animcommon.h"
#include "animeventobserver.h"

#include "pnirefcountdbg.h"

/////////////////////////////////////////////////////////////////////

namespace anim {

  class group;
  class manager;

/////////////////////////////////////////////////////////////////////

class base :
  public pni::pstd::refCount,
  public idBase
{
    friend class group;

  public:
    base();
//     base(base const& rhs);
//     base& operator=(base const& rhs);
//     bool operator==(base const& rhs) const;
    
      // Note: xforms are multiplied to go up the anim graph
      // and inverse xforms are multiplied to push values down
      // the anim graph.
    void setXform ( xform const& val );
    xform& xformOp () { setDirty (); return mXform; }
    xform& getXform () { return mXform; }
    xform const& getXform () const { return mXform; }
    
      // Looping is specified as a time duration that is 
      // evaluated during the bounds of the animation.
      // So, a 10 sec animation with a loop of 5 secs will
      // run through the animation twice.
      
      // GOTCHA: Currently this is only used on anim::groups.
      // How to get looping to work:
      // Set group to have loopDuration... the group will cycle with this value.
      // Set children to have duration 1.0.
      // This works for at least a single child.
      // TODO: Must review this for ease of use and correctness later.
    void setLoopDuration ( TimeType val ) { mLoopDuration = val; }
    TimeType getLoopDuration () const { return mLoopDuration; }
    
      // [0,.5] =POWER(tval*2),exponent)/2
      // (.5,1] =1-POWER(ABS(tval*2-2),exponent)/2
    void setExponent ( float val = 1.0f ) { mExponent = val; }
    float getExponent () const { return mExponent; }
    
      ///////
      // Bounds:
      // GOTCHA: Note that bounds are always updated to be in the parent
      // frame of reference.  Go figure.  This is unlike how nodes keep
      // their bounds in their own frame.
      
      // Bounds override is used to clip an animation.
    void setBoundsOverride ( bool val ) { mBoundsOverride = val; }
    void setBounds ( bounds const& bounds ) { mBounds = bounds; }
      
    bounds const& getBounds () const { updateBounds (); return mBounds; }
    bool contains ( TimeType now ) const
        {
          updateBounds ();
          return mBounds.contains ( now );
        }
    bool isDone ( TimeType now ) const
        {
          updateBounds ();
          return ( now >= mBounds.mStart + mBounds.mDuration );
        }

    void update ( manager* pManager, TimeType curTime );
    
      // Allows things that use matrixHelper to push results
      // to node/widget.
    virtual void finalize () {}
    virtual bool needsFinalize () const { return false; }
    
      // Control whether animations are thrown away after completion.
      // Default is true.
      // TRICKY: Currently, when animations are not disposed, and they
      // end before their owning group, they will continue to get
      // evaluated at their end tval until the group completes.
      // Possibly changing the way that completion is evaluated will
      // fix this.
    void setDoGc ( bool val ) { mDoGc = val; }
    bool getDoGc () const { return mDoGc; }
    
     
     // add anim observers
    void setAnimObserver ( animObserver* pObs ) { mObserver = pObs; }
    animObserver* getAnimObserver () const { return mObserver; }
     
  protected:
    virtual ~base();

    group* mParent;
  
    xform mXform;
    mutable bounds mBounds;
    animObserver* mObserver;
    float mExponent;
    TimeType mLoopDuration;
    bool mBoundsOverride;
    bool mDoGc;
    
    virtual void setDirty ( bool val = true ) const;
    virtual void updateBounds () const;
    virtual void internalUpdate ( manager* pManager, TimeType tval ) = 0;
    
    float useExponent ( manager* pManager, float val );

  private:
    

  // interface from pni::pstd::refCount
  public:
    
  protected:
    
    // From refCount
  protected:
    virtual void onDoDbg ();
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimbase_h


