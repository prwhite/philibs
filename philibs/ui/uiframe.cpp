/////////////////////////////////////////////////////////////////////
//
//    file: uiframe.cpp
//
/////////////////////////////////////////////////////////////////////

#include "uiframe.h"
#include "pnirefcountdbg.h"

/////////////////////////////////////////////////////////////////////

namespace ui {
  
/////////////////////////////////////////////////////////////////////

frame::frame()
{
  //printf ( "frame::frame %x\n", this );
}

frame::~frame()
{
//printf ( "frame::~frame %x\n", this );

    // Important to clean up children's parent pointers.
  while ( ! mChildren.empty () )
  {
    remChild ( mChildren.back ().get () );
  }
}

//frame::frame(frame const& rhs)
//{
//  
//}
//
//frame& frame::operator=(frame const& rhs)
//{
//  
//  return *this;
//}
//
//bool frame::operator==(frame const& rhs) const
//{
//  
//  return false;
//}

//void removeAllParents ( scene::node* pNode )
//{
//  typedef scene::node::Nodes Nodes;
//  Nodes& ps = pNode->getParents ();
//  
//  Nodes::iterator end = ps.end ();
//  for ( Nodes::iterator cur = ps.begin ();
//      cur != end;
//      ++cur )
//  {
//    ( *cur )->remChild ( pNode );
//  }
//}

void frame::addChild ( base* pBase )
{
  mChildren.push_back ( pBase );
  pBase->mParent = this;
  size_t numParents = pBase->mBaseNode.get ()->getParents ().size ();
  
  if ( ! pBase->isMegaGui () || numParents == 0 )
    mBaseNode->addChild ( pBase->mBaseNode.get () );
}

void frame::remChild ( base* pBase )
{
  pBase->mParent = 0;

  Bases::iterator found = 
    std::find ( mChildren.begin (), mChildren.end (), pBase );
  if ( found != mChildren.end () )
  {
    // TRICKY This is not symmetric with addChild for the
    // mega-gui case.
    //if ( ! pBase->isMegaGui () )
      mBaseNode->remChild ( ( *found )->mBaseNode.get () );
    mChildren.erase ( found );
  }
}


/////////////////////////////////////////////////////////////////////
// overrides from base

void frame::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  base::collectRefs ( refs );
  pni::pstd::dbgRefCount::collectVecRefs ( mChildren, refs );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace ui 


