/////////////////////////////////////////////////////////////////////
//
//    file: scenenode.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenenode.h"
#include <algorithm>

#include "pnirefcountdbg.h"

#include <iostream>

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

node::node()
{
  mTravMask.resize ( TravCount, 0xffffffff );
}

node::~node()
{
//printf ( "~node\n" );
  remAllChildren ();  // not redundant because it also clears parent
                      // pointers.
}

//     matrix4 mMatrix;
//    mutable box3 mBounds;
//    Nodes mChildren;
//    Nodes mParents;
//    States mStates;
//      unsigned int mTravMask[ TravCount ];
//      std::string mName;

node::node(node const& rhs) :
  mMatrix ( rhs.mMatrix ),
  mBounds ( rhs.mBounds ),
  mTravMask ( rhs.mTravMask ),
  mName ( rhs.mName ),
  mStates ( rhs.mStates ) // Refernce source states.
{
  // Don't do this in the basic copy constructor... just
  // alias all states for now... a smarter traversal can
  // make the state unique.

//   States::const_iterator end = rhs.mStates.end ();
//   for ( States::const_iterator cur = rhs.mStates.begin ();
//     cur != end;
//     ++cur )
//   {
//     setState ( cur->second.get ()->dup (), cur->first );
//   }
}

//node& node::operator=(node const& rhs)
//{
//    
//    return *this;
//}
//
//bool node::operator==(node const& rhs) const
//{
//    
//    return false;
//}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

inline
void node::setParentBoundsDirty ()
{
  Parents::iterator end = mParents.end ();
  for ( Parents::iterator cur = mParents.begin ();
    cur != end;
    ++cur )
  {
    ( *cur )->setBoundsDirty ();
  }
}

/////////////////////////////////////////////////////////////////////

void node::setBoundsDirty ()
{
  mBounds.setIsDirty ( true );

//printf ( "set bounds dirty for %x w/name %s\n", this, getName ().c_str () );
  setParentBoundsDirty ();  
}

/////////////////////////////////////////////////////////////////////

box3 const& node::getBounds () const
{
  if ( mBounds.getIsDirty () )
  {
    updateBounds ();
  }
  
  return mBounds;
}

/////////////////////////////////////////////////////////////////////

void node::updateBounds () const
{
  mBounds.setEmpty ();

  Nodes::const_iterator end = mChildren.end ();
  for ( Nodes::const_iterator cur = mChildren.begin ();
      cur != end;
      ++cur )
  {
    node* pNode = cur->get ();
    pNode->updateBounds ();

    pni::math::matrix4 mat = pNode->getMatrix ();
    pni::math::box3 box = pNode->getBounds ();

    box.xform ( box, mat );
    mBounds.extendBy ( box );
  }
  
  mBounds.setIsDirty ( false );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void node::getCenterInParent ( pni::math::vec3& center ) const
{
  getBounds ().getCenter ( center );
  center.xformPt4 ( center, mMatrix );
}

/////////////////////////////////////////////////////////////////////

  // The non-const methods automatically call setBoundsDirty.
  // Maybe that's overzealous for getChildren.
void node::addChild ( node* pNode )
{
  setBoundsDirty ();
  mChildren.push_back ( pNode );
  
  pNode->mParents.push_back ( this );
}

void node::remChild ( node* pNode )
{
  setBoundsDirty ();
  Parents::iterator pfound = 
    std::find ( pNode->mParents.begin (), pNode->mParents.end (), this );
  if ( pfound != pNode->mParents.end () )
  {
    pNode->mParents.erase ( pfound );
  }

  Nodes::iterator found = 
    std::find ( mChildren.begin (), mChildren.end (), pNode );
  if ( found != mChildren.end () )
  {
    mChildren.erase ( found );
  }
}

void node::remAllChildren  ()
{
    // Unoptimal
    
  //typedef Nodes::reverse_iterator Iter;
  //for ( Iter cur = mChildren.rbegin ();
  //    cur != mChildren.rend ();
  //    ++cur )
  //  remChild ( cur->get () );
  
  while ( ! mChildren.empty () )
    remChild ( mChildren[ 0 ].get ()  );
}

void node::remParent ( node* pNode )
{
  Parents::iterator pfound = 
    std::find ( pNode->mParents.begin (), pNode->mParents.end (), pNode );
  if ( pfound != pNode->mParents.end () )
  {
    ( *pfound )->remChild ( this );
  }  
}

void node::remAllParents ()
{
    // Unoptimal.
  //typedef Parents::reverse_iterator Iter;
  //for ( Iter cur = mParents.rbegin (); 
  //    cur != mParents.rend (); 
  //    ++cur )
  //{
  //  ( *cur )->remChild ( this );
  //}
  
  Parents tParents = mParents;
  typedef Parents::iterator Iter;
  for ( Iter cur = tParents.begin (); 
      cur != tParents.end (); 
      ++cur )
  {
    ( *cur )->remChild ( this );
  }
}

////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void node::setState ( state* pState, state::Id id )
{
//   assert ( pState );
  
    // Must protect against set of zero... rather do it
    // here rather than every frame we evaluate states.
  if ( pState )
    mStates[ id ] = pState;
  else
    remState ( id );
}

void node::remState ( state* pState )
{
  States::iterator end = mStates.end ();
  for ( States::iterator cur = mStates.begin ();
      cur != end;
      ++cur )
  {
    if ( cur->second == pState )
    {
      mStates.erase ( cur );
      return;
    }
  }
}

void node::remState ( state::Id id )
{
  States::iterator found =  mStates.find ( id );
  if ( found != mStates.end () )
    mStates.erase ( found );  
}

state* node::getState ( state::Id id ) const
{
  States::const_iterator found = mStates.find ( id );
  if ( found != mStates.end () )
    return found->second.get ();
  else
    return 0;
}

void node::uniquifyStates ()
{
  States::iterator end = mStates.end ();
  for ( States::iterator cur = mStates.begin ();
      cur != end;
      ++cur )
  {
    if ( cur->second && ( cur->second->getNumRefs () > 1 ) )
    {
      setState ( cur->second->dup (), cur->first );
    }
  }
}

void node::uniquifyState ( state::Id id )
{
  if ( state* pState = getState ( id ) )
    setState ( pState->dup (), id );
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void node::onDoDbg ()
{
  std::ostream* pStr = pni::pstd::dbgRefCount::getStr ();
  
  *pStr << "   mName = " << mName;
}

    //Nodes mChildren;
    //States mStates;

void node::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  pni::pstd::dbgRefCount::collectVecRefs ( mChildren, refs );
  pni::pstd::dbgRefCount::collectMapSecondRefs ( mStates, refs );
  
  pni::pstd::dbgRefCount::collectVecRefs ( getTravDatum (), refs );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace scene 


