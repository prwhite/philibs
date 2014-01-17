/////////////////////////////////////////////////////////////////////
//
//    file: sceneloaderbase.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneloaderbase.h"

#include "pnirefcountdbg.h"

/////////////////////////////////////////////////////////////////////

namespace loader {

/////////////////////////////////////////////////////////////////////

void cache::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  pni::pstd::dbgRefCount::collectMapSecondRefs ( mTextures, refs );
  refs.push_back ( mDefBlend.get () );
}

/////////////////////////////////////////////////////////////////////

void directory::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  pni::pstd::dbgRefCount::collectMapSecondRefs ( mNodes, refs );
}

/////////////////////////////////////////////////////////////////////

base::base() :
  mCache ( new cache ),
  mDirectory ( new directory ),
  mSearchPath ( new pni::pstd::searchPath )
{
  mObserver = this;
  setProgressObserver ( this );
  mSearchPath->addPath(".");
}

base::~base()
{
  
}

// base::base(base const& rhs)
// {
//     
// }
// 
// base& base::operator=(base const& rhs)
// {
//     
//     return *this;
// }
// 
// bool base::operator==(base const& rhs) const
// {
//     
//     return false;
// }

/////////////////////////////////////////////////////////////////////

scene::node* directory::getNode ( std::string const& name )
{
  NodeIter found = mNodes.find ( name );
  if ( found != mNodes.end () )
    return found->second.get ();
  else
    return 0;
}
    
/////////////////////////////////////////////////////////////////////

      // Gets lower/upper iterators for node name query.
directory::NodeRange directory::getNodes ( std::string const& name )
{
  return NodeRange ( 
      mNodes.lower_bound ( name ), 
      mNodes.upper_bound ( name ) );
}

/////////////////////////////////////////////////////////////////////

void directory::addNode ( scene::node* pNode )
{
  if ( mEnable )
    mNodes.insert ( Nodes::value_type ( pNode->getName (), pNode ) );
}

/////////////////////////////////////////////////////////////////////

void directory::addScene ( scene::node* pNode )
{
  if ( ! pNode )
    return;

  addNode ( pNode );
  
  typedef scene::node::Nodes::const_iterator KidIter;
  
  KidIter end = pNode->getChildren ().end ();
  for ( KidIter cur = pNode->getChildren ().begin ();
      cur != end;
      ++ cur )
  {
    addScene ( cur->get () );
  }
}

/////////////////////////////////////////////////////////////////////

void directory::gc ()
{
  NodeIter end = mNodes.end ();
  for ( NodeIter cur = mNodes.begin (); cur != end;  )
  {
    if ( cur->second->getNumRefs () == 1 )
    {
      NodeIter gone = cur;
      ++cur;
      mNodes.erase ( gone );
    }
    else
      ++cur;
  }
}

/////////////////////////////////////////////////////////////////////

void base::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  refs.push_back ( mDirectory.get () );
  refs.push_back ( mCache.get () );
  refs.push_back ( mSearchPath.get () );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace loader 


