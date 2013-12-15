/////////////////////////////////////////////////////////////////////
//
//    class: sceneNodeTrans
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimscenenodetrans_h
#define animanimscenenodetrans_h

/////////////////////////////////////////////////////////////////////

#include "animtrans.h"

#include "animmatrixhelper.h"
#include "animmanager.h"
#include "animcommon.h"

#include "pniautoref.h"

#include "pnivec3.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace anim {
  
/////////////////////////////////////////////////////////////////////

class sceneNodeTrans :
  public trans,
  public matrixHelperNode
{
  public:
//     sceneNodeTrans();
//     virtual ~sceneNodeTrans();
//     sceneNodeTrans(sceneNodeTrans const& rhs);
//     sceneNodeTrans& operator=(sceneNodeTrans const& rhs);
//     bool operator==(sceneNodeTrans const& rhs) const;'

  protected:
    
  private:

    // From anim::base
  public:
    
    virtual void finalize ()
        {
          mNode->matrixOp () = this->mHelper->updateMatrix ();
        }

  protected:
    
    // interface from matrixHelperNode
  public:
    virtual scene::matrix4& matrixOp () { return matrixHelperNode::matrixOp (); }
    virtual scene::matrix4 const& getMatrix () const { return matrixHelperNode::getMatrix (); }

  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimscenenodetrans_h


