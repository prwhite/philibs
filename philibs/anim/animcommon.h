/////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////

#ifndef animcommon_h
#define animcommon_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "pniautoref.h"

#include "scenenode.h"
#include "uibase.h"

#include "animmatrixhelper.h"

/////////////////////////////////////////////////////////////////////

namespace anim {

  typedef float TimeType;


/////////////////////////////////////////////////////////////////////

struct xform
{
  xform () : mStart ( 0.0f ), mDuration  ( 1.0f ) {}
  xform ( TimeType start, TimeType duration ) :
    mStart ( start ), mDuration ( duration ) {}

  void invert ()
      {
        // UNTESTED.
        mStart = -mStart / mDuration;
        mDuration = 1.0f / mDuration;
      }
      
  void setIdentity ()
      {
        mStart = 0.0f;  mDuration = 1.0f;
      }
      
  TimeType transform ( TimeType now ) const
      {
        return mStart + now * mDuration;
      }
      
  TimeType getEnd () const { return mStart + mDuration; }

  TimeType mStart;
  TimeType mDuration;
};

/////////////////////////////////////////////////////////////////////

struct bounds
{
  bounds () : mStart ( 0.0f ), mDuration  ( 0.0f ) {}
  bounds ( TimeType start, TimeType duration ) :
    mStart ( start ), mDuration ( duration ) {}

  void set ( xform const& rhs )
      {
        mStart = rhs.mStart;
        mDuration = rhs.mDuration;
      }

  void extendBy ( bounds const& val )
      {
        extendBy ( val.mStart );
        extendBy ( val.mStart + val.mDuration );
      }

  void extendBy ( TimeType val )
      {
        if ( val < mStart )
          mStart = val;
        else if ( val > mStart + mDuration )
          mDuration = val - mStart;
      }
      
  bool contains ( TimeType now ) const
      {
        return ( now >= mStart 
          && now < ( mStart + mDuration ) );
      }
      
  void transform ( xform const& val )
      {
        mStart = val.mStart + mStart * val.mDuration;
        mDuration *= val.mDuration;
      }

  void invalidate ()
      {
        mStart = 0.0f;
        mDuration = 0.0f;
      }

  TimeType getEnd () const { return mStart + mDuration; }

  TimeType mStart;
  TimeType mDuration;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

  // This class is mixed in to classes derived from anim::base that
  // will manipulate matrices by separate trans, rot and scale inputs.
  // It provides concrete management of a matrixHelper ref.
class matrixHelperBase
{
  public:
  
    virtual void setMatrixHelper ( matrixHelper* pHelper ) { mHelper = pHelper;  }
    matrixHelper* getMatrixHelper () const { return mHelper.get (); }
    
  protected:
    typedef pni::pstd::autoRef< matrixHelper > HelperRef;
    HelperRef mHelper;

    virtual ~matrixHelperBase () {}
    
  private:

};

/////////////////////////////////////////////////////////////////////

  // This class is mixed in to classes to provide concrete management
  // of node references... e.g., a common interface to managing node
  // targets, without putting together a full abstraction.
class matrixHelperNode
{
  public:
  
    //void finalizeNode ()
    //    {
    //      mNode->matrixOp () = this->mHelper->updateMatrix ();
    //    }
        
    scene::matrix4& matrixOp () { return mNode->matrixOp (); }
    scene::matrix4 const& getMatrix () const { return mNode->getMatrix (); }
    
    typedef pni::pstd::autoRef< scene::node > NodeRef;
    virtual void setNode ( scene::node* pNode ) { mNode = pNode; }
    scene::node* getNode () const { return mNode.get (); }
    
  protected:
    NodeRef mNode;
};

/////////////////////////////////////////////////////////////////////

  // This class is mixed in to classes to provide concrete management
  // of ui references... e.g., a common interface to managing ui
  // targets, without putting together a full abstraction.
class matrixHelperUiBase
{
  public:
  
    //void finalizeUiBase ()
    //    {
    //      mUiBase->matrixOp () = this->mHelper->updateMatrix ();
    //    }
    
    scene::matrix4& matrixOp () { return mUiBase->matrixOp (); }
    scene::matrix4 const& getMatrix () const { return mUiBase->getMatrix (); }

    typedef pni::pstd::autoRef< ui::base > UiBaseRef;
    virtual void setUiBase ( ui::base* pBase ) { mUiBase = pBase; }
    ui::base* getUiBase () const { return mUiBase.get (); }
    
  protected:
    UiBaseRef mUiBase;
};

/////////////////////////////////////////////////////////////////////

} // end namespace anim

/////////////////////////////////////////////////////////////////////

#endif // animcommon_h
