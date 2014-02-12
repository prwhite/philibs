/////////////////////////////////////////////////////////////////////
//
//    file: animscenenodevis.cpp
//
/////////////////////////////////////////////////////////////////////

#include "animscenenodevis.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

sceneNodeVis::sceneNodeVis () :
  mMask ( 0 )
{
  // TODO
}

//sceneNodeVis::~sceneNodeVis ()
//{
//  // TODO
//}
//
//sceneNodeVis::sceneNodeVis ( sceneNodeVis const& rhs )
//{
//  // TODO
//}
//
//sceneNodeVis& sceneNodeVis::operator= ( sceneNodeVis const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool sceneNodeVis::operator== ( sceneNodeVis const& rhs ) const
//{
//  // TODO
//  return false;
//}

/////////////////////////////////////////////////////////////////////
//  overrides from anim::base

void sceneNodeVis::internalUpdate ( manager* pManager, TimeType tval )
{
    // We assume that tval will always be ~1.0 here because all
    // one-shot anims should have a duration of 0, causing the 
    // anim manager to call update with 1.0.
    // Just for kicks, we'll make sure it's close to 1.0.
  if ( tval >= 0.99f )    // Magic Number!!!
    mNode->setTravMask ( scene::Draw, mMask );
}


/////////////////////////////////////////////////////////////////////

void sceneNodeVis::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  base::collectRefs ( refs );
  refs.push_back ( mNode.get () );
}

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 


