/////////////////////////////////////////////////////////////////////
//
//    file: animscenenodetrans.cpp
//
/////////////////////////////////////////////////////////////////////

#include "animscenenodetrans.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
  
/////////////////////////////////////////////////////////////////////

// sceneNodeTrans::sceneNodeTrans()
// {
//   
// }
// 
// sceneNodeTrans::~sceneNodeTrans()
// {
//   
// }
// 
// sceneNodeTrans::sceneNodeTrans(sceneNodeTrans const& rhs)
// {
//   
// }
// 
// sceneNodeTrans& sceneNodeTrans::operator=(sceneNodeTrans const& rhs)
// {
//   
//   return *this;
// }
// 
// bool sceneNodeTrans::operator==(sceneNodeTrans const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////

void sceneNodeTrans::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  trans::collectRefs ( refs );
  refs.push_back ( mNode.get () );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace anim 


