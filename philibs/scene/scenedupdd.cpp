/////////////////////////////////////////////////////////////////////
//
//    file: scenedupdd.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenedupdd.h"

#include "pnimathstream.h"

#include "scenecamera.h"
#include "scenegeom.h"
#include "scenegeomfx.h"
#include "scenegroup.h"
#include "scenelight.h"
#include "scenesndeffect.h"
#include "scenesndlistener.h"

#include "sceneblend.h"
#include "scenecull.h"
#include "scenedepth.h"
#include "scenelighting.h"
#include "scenelightpath.h"
#include "scenematerial.h"
// #include "scenepolygonmode.h"
#include "scenetexenv.h"
// #include "scenetexgen.h"
#include "scenetexture.h"
#include "scenetexturexform.h"


/////////////////////////////////////////////////////////////////////

using namespace std;

namespace scene {
  
/////////////////////////////////////////////////////////////////////

dupDd::dupDd () :
  mOpts ( None ),
  mCurId ( state::Blend ) // Bad, maybe horrible default.
{
  
}

//dupDd::~dupDd()
//{
//    
//}
//
//dupDd::dupDd(dupDd const& rhs)
//{
//    
//}
//
//dupDd& dupDd::operator=(dupDd const& rhs)
//{
//    
//    return *this;
//}
//
//bool dupDd::operator==(dupDd const& rhs) const
//{
//    
//    return false;
//}

/////////////////////////////////////////////////////////////////////
// overrides from graphDd

//void dupDd::indent ( std::string const& str )
//{
//  for ( size_t num = 0; num < mIndent; ++num )
//    mStr << str;
//}

void dupDd::startGraph ( node const* pNode )
{
  
}

node* dupDd::dupGraph ( node const* pNode )
{
    // Do reset work.
  mNodeStack.clear ();
  mRetNode = 0;
  
  pNode->accept ( this );
  
  return mRetNode;
}


void dupDd::dispatchChildren ( node const* pNode )
{
  typedef node::Nodes::const_iterator Iter;
  Iter end = pNode->getChildren ().end ();
  for ( Iter cur = pNode->getChildren ().begin ();
      cur != end;
      ++cur )
  {
    ( *cur )->accept ( this );
  }
}

/////////////////////////////////////////////////////////////////////

void dupDd::dispatchNodePre ( node const* pNode )
{
  node* pDup = pNode->dup ();
  if ( ! mNodeStack.empty () )
    mNodeStack.back ()->addChild ( pDup );
  mNodeStack.push_back ( pDup );
  if ( ! mRetNode )
    mRetNode = pDup;
}

void dupDd::dispatchNodePost ( node const* pNode )
{
  mNodeStack.pop_back ();
}


void dupDd::dispatchNode ( node const* pNode )
{
  startStates ( pNode );
}

void dupDd::dispatchNodePush ( node const* pNode )
{
  dispatchNodePre ( pNode );
  dispatchNode ( pNode );
  dispatchNodePost ( pNode );
}

/////////////////////////////////////////////////////////////////////

void dupDd::dispatch ( camera const* pNode )
{
  dispatchNodePush ( pNode );
}

/////////////////////////////////////////////////////////////////////

void dupDd::dispatch ( geom const* pNode )
{
  dispatchNodePre ( pNode );
  dispatchNode ( pNode );
  if ( mOpts & UniquifyGeomData )
  {
    geom* pGeom = static_cast< geom* > ( mNodeStack.back () );
    pGeom->uniquifyGeometry ();
  }
      
  dispatchNodePost ( pNode );
}

/////////////////////////////////////////////////////////////////////

void dupDd::dispatch ( geomFx const* pNode )
{
  dispatch ( ( geom* ) pNode );
}

/////////////////////////////////////////////////////////////////////

void dupDd::dispatch ( group const* pNode )
{
  dispatchNodePre ( pNode );

  dispatchNode ( pNode );
  dispatchChildren ( pNode );

  dispatchNodePost ( pNode );
}

/////////////////////////////////////////////////////////////////////

void dupDd::dispatch ( light const* pNode )
{
  dispatchNodePush ( pNode );
}

/////////////////////////////////////////////////////////////////////

void dupDd::dispatch ( sndEffect const* pNode )
{
  dispatchNodePush ( pNode );
}

/////////////////////////////////////////////////////////////////////

void dupDd::dispatch ( sndListener const* pNode )
{
  dispatchNodePush ( pNode );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void dupDd::startStates ( node const* pNode )
{
    // The normal node dup operation will alias/ref the original
    // node's states.  So, only go through duping states if uniquify
    // states is on.  And, there's an easy way to do that.  So,
    // this gets to be really simple.
  if ( ! ( mOpts & UniquifyStates ) )
    return;

  mNodeStack.back ()->uniquifyStates ();
  
    // This is the long-hand way to do the uniquify op.  We won't
    // be needing that because of node::uniquifyStates.
  //typedef node::States::const_iterator Iter;
  //
  //Iter end = pNode->getStates ().end ();
  //for ( Iter cur = pNode->getStates ().begin (); cur != end; ++cur )
  //{
  //  mCurId = cur->first;
  //  cur->second->accept ( this );
  //}
}

void dupDd::setDefaultState ( state const* pState, state::Id id )
{
  // No-op.
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void dupDd::dispatchState ( state const* pState )
{
    // Shouldn't be here without a valid node on the stack,
    // so we don't check. ;)
  node* pNode = mNodeStack.back ();
  
  pNode->setState ( pState->dup (), mCurId );
}

void dupDd::dispatch ( blend const* pState )
{
  dispatchState ( pState );
}

void dupDd::dispatch ( cull const* pState )
{
  dispatchState ( pState );
}

void dupDd::dispatch ( depth const* pState )
{
  dispatchState ( pState );
}

void dupDd::dispatch ( lighting const* pState )
{
  dispatchState ( pState );
}

void dupDd::dispatch ( lightPath const* pState )
{
  dispatchState ( pState );
}

void dupDd::dispatch ( material const* pState )
{
  dispatchState ( pState );
}

void dupDd::dispatch ( texEnv const* pState )
{
  dispatchState ( pState );
}

void dupDd::dispatch ( texture const* pState )
{
  dispatchState ( pState );
}

void dupDd::dispatch ( textureXform const* pState )
{
  dispatchState ( pState );
}



} // end of namespace scene 


