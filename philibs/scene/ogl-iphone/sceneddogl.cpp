/////////////////////////////////////////////////////////////////////
//
//    file: scenegraphddogl.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PSTDPROFDISABLE

#include "sceneddogl.h"

#include "scenecamera.h"
#include "scenegeom.h"
#include "scenegeomfx.h"
#include "scenegroup.h"
#include "scenelight.h"
#include "scenetext.h"

#include "pnitimerprof.h"

#include "sceneconverter.h"

#include "pnidbg.h"


/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

ddOgl::ddOgl()
{
  // Fill the bottom of the stack with an empty entry.
  const size_t Num = 32;
  mStateStack.reserve ( Num );
  mStateStack.push_back ( stateSet () );
  
  mMatStack.push ();
  
//   mMatStack.reserve ( Num );
//   mMatInvStack.reserve ( Num );
  
#ifdef USEBOUNDSSTACK
  mBoundsStack.reserve ( Num );
#endif // USEBOUNDSSTACK
}

ddOgl::~ddOgl()
{
  // Free all leftover state in stack.
  while ( ! mStateStack.empty () )
  {
    mStateStack.pop_back ();
  }
}

// ddOgl::ddOgl(ddOgl const& rhs)
// {
//   
// }
// 
// ddOgl& ddOgl::operator=(ddOgl const& rhs)
// {
//   
//   return *this;
// }
// 
// bool ddOgl::operator==(ddOgl const& rhs) const
// {
//   
//   return false;
// }

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Graph core traversal methods.

void ddOgl::startGraph ( node const* pNode )
{
  if ( ! pNode )
    return;

#ifndef _NDEBUG
  if ( mDbgVals )
  {
    printf ( "ddOgl: %p Frame Start on node %s \n", this, pNode->getName ().c_str () );
  }
#endif // _NDEBUG

#ifndef _NDEBUG
  mDdList.setDbgVals ( getDbgVals () );
#endif // _NDEBUG

PNIDBG
static pni::pstd::timerProf trav ( "graph traversal" );

  // FIXME Should be real error handling somewhere.
  if ( mSinkPath.empty () )
    return;

PNIDBG
  setupCamera ( pNode );
PNIDBG
  // clear any old render list.
// EXPERIMENTAL!!!
  //mDdList.resetRenderItems ();
PNIDBG
  // This actually fills in ddOglList which does the real OpenGL 
  // goodness.
  
  mNodePath.push ( const_cast< node* > ( pNode ) );
  
PSTDPROFCALL(trav.start ());
  pNode->accept ( this );
PSTDPROFCALL(trav.stop ());
PNIDBG

  mNodePath.pop ();

  // Now trigger the real rendering.
  mDdList.setSinkPath ( getSinkPath () );

static pni::pstd::timerProf list ( "list traversal" );
PSTDPROFCALL(list.start ());
  mDdList.startList ();
PSTDPROFCALL(list.stop ());

// EXPERIMENTAL!!!
    // TODO: Put into cleanup method.
  mDdList.resetRenderItems ();
  mNodePath.clear ();

#ifndef _NDEBUG
  if ( mDbgVals )
  {
    printf ( "ddOgl: %p Frame End on node %s \n", this, pNode->getName ().c_str () );
  }
#endif // _NDEBUG

PNIDBG
}

/////////////////////////////////////////////////////////////////////

//void ddOgl::setupCamera ( node const* pNode )
//{
//  // TODO set up things like the frustum for culling, etc.
//  
//    // First setup camera position for z-sort.
//    // dst should be camera position in world coordinates.
//  pni::math::vec3 src;  // Default to 0,0,0 which is camera origin in
//                        // its local coordinate frame.
//  pni::math::vec3 dst;
//      
//  scene::converter conv;
//  conv.convert ( dst, src, mSinkPath, 
//      nodePath ( const_cast< node* > ( pNode ) ) );
//      
//  mCamPos = dst;
//}

void ddOgl::setupCamera ( node const* pNode )
{
  // TODO set up things like the frustum for culling, etc.
  
    // Get viewport so we have something sensible to send in
    // to screenToWorld.
  scene::camera* pCam = static_cast< scene::camera* > ( 
      mSinkPath.getLeaf () );
  
  float left, bottom, width, height;  // Uninitialized!
  
  pCam->getViewport ( left, bottom, width, height );
  
    // Setup camera position for z-sort.
    // dst should be camera position in world coordinates.
  pni::math::vec3 src ( width / 2.0f, height / 2.0f, -1.0f );
  pni::math::vec3 dst;
      
  scene::converter conv;
  conv.screenToWorld ( dst, src, 
      nodePath ( const_cast< node* > ( pNode ) ),
      mSinkPath );

  mCamPos = dst;
  
#ifndef _NDEBUG
  if ( mDbgVals & DbgSort )
  {
    printf ( "DbgSort: mCamPos = %f, %f, %f\n",
        mCamPos[ 0 ], mCamPos[ 1 ], mCamPos[ 2 ] );
  }
#endif  // _NDEBUG
}

/////////////////////////////////////////////////////////////////////

float ddOgl::calcDistSqr ( node const* pNode )
{
    // Take center of this node's bbox.
  pni::math::vec3 center ( pni::math::vec3::NoInit );
  pNode->getBounds ().getCenter ( center );
    
    // Transform to world coordinates.
  center.xformPt4 ( center, *mMatStack );

    // Then get distSqr from mCamPos.
  float distSqr = mCamPos.distSqr ( center );

#ifndef _NDEBUG
  if ( mDbgVals & DbgSort )
  {
    printf ( "DbgSort: node %s pos = %f, %f, %f dist = %f\n",
        pNode->getName ().c_str (), center[ 0 ], center[ 1 ], center[ 2 ],
        distSqr );
  }
#endif  // _NDEBUG


  return distSqr;
}

/////////////////////////////////////////////////////////////////////

bool ddOgl::dispatchPre ( node const* pNode )
{  
PNIDBG
  // Check draw mask.
  if ( ! ( getTravMask() & pNode->getTravMask ( Draw ) ) )
    return false;
  
static pni::pstd::timerProf aa ( "mat stack", 120 * 5 );
PSTDPROFCALL(aa.start ());

PNIDBG
  // push matrix if not identity
  if ( ! pNode->getMatrix ().getIsIdent () )
  {
PNIDBG
    mMatStack.push ();
    mMatStack->preMult ( pNode->getMatrix () );
//     mMatStack->postMult ( pNode->getMatrix () );

#ifdef USEINVMAT
    mMatInvStack.push ();
    *mMatInvStack = *mMatStack;
    mMatInvStack->invert ();
#endif // USEINVMAT

PNIDBG
  }
PSTDPROFCALL(aa.stop ());

static pni::pstd::timerProf bb ( "bounds stack", 120 * 5 );
PSTDPROFCALL(bb.start ());

#ifdef USEBOUNDSSTACK
    // TODO: Can optimize out one copy here by pushing, 
    // and then doing mat mult onto back.
  // push bounds on stack?
  //pni::math::box3 tbox = pNode->getBounds ();
  //tbox.xform ( tbox, *mMatStack );
  //mBoundsStack.push_back ( tbox );
  mBoundsStack.push_back ( pNode->getBounds () );
  mBoundsStack.back ().xform ( mBoundsStack.back (), *mMatStack );
#endif // USEBOUNDSSTACK

PSTDPROFCALL(bb.stop ());

  // Check bounds in view frustum.
  // Kinda lame to undo all of the pushes in an asymmetric way,
  // but it does keep us from doing some work twice and passing 
  // tons of parameters to the cull function.  At least it is
  // all localized here.
PNIDBG
  if ( ! testInViewFrustum ( pNode ) )
  {
PNIDBG
    mMatStack.pop ();
#ifdef USEINVMAT
    mMatInvStack.pop ();
#endif // USEINVMAT
#ifdef USEBOUNDSSTACK
    mBoundsStack.pop_back ();
#endif // USEBOUNDSSTACK
PNIDBG
    return false;
  }
  
static pni::pstd::timerProf cc ( "states", 120 * 5 );
PSTDPROFCALL(cc.start ());

PNIDBG
  // push state stack if states on node
  if ( ! pNode->getStates ().empty () )
  {
PNIDBG
    mStateStack.push_back ( mStateStack.back () );
    applyStates ( pNode );
PNIDBG
  }
PSTDPROFCALL(cc.stop ());
  
PNIDBG
  return true;
}

/////////////////////////////////////////////////////////////////////

bool ddOgl::testInViewFrustum ( node const* pNode )
{
  // TODO
  
  return true;
}

/////////////////////////////////////////////////////////////////////

void ddOgl::applyStates ( node const* pNode )
{
PNIDBG
    // Source
  node::States const& src = pNode->getStates ();
  
    // Destination
  stateSet& dst = mStateStack.back ();
  
    // Loop over source
  for ( auto& cur : src )
  {
      // Vars for inner loop
    if ( cur.second->getTravMask () & getTravMask() )
    {
      state::Id id = cur.first;
      state const* const pDstState = dst.mStates[ id ];
      
        // Adopt new state if current stack top for this slot
        // is null or if it is valid and not overriding.
      if ( ( ! pDstState ) || 
          ( pDstState && ! pDstState->getOverride () ) )
      {
        dst.mStates[ id ] = cur.second.get ();
      }
    }
  }
PNIDBG
}

/////////////////////////////////////////////////////////////////////

void ddOgl::dispatchChildren ( node const* pNode )
{
  for ( auto& cur : pNode->getChildren())
  {
    mNodePath.push ( const_cast< node* > ( cur.get () ) );
    cur->accept ( this );
    mNodePath.pop ();
  }
}

/////////////////////////////////////////////////////////////////////

void ddOgl::dispatchPost ( node const* pNode )
{
PNIDBG
  // pop matrix if not identity
  if ( ! pNode->getMatrix ().getIsIdent () )
  {
    mMatStack.pop ();
#ifdef USEINVMAT
    mMatInvStack.pop ();
#endif // USEINVMAT
  }
    
  // pop state stack if states on node
  if ( ! pNode->getStates ().empty () )
    mStateStack.pop_back ();

  // pop bounds off stack
#ifdef USEBOUNDSSTACK
  mBoundsStack.pop_back ();
#endif // USEBOUNDSSTACK
PNIDBG
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// For no particular reason, only group is currently traversing its
// children.  It's kinda arbitrary, and weird w/rt the node interface
// which gives all nodes a child list.  LAME PRW

void ddOgl::dispatch ( camera const* pNode )
{
//   if ( dispatchPre ( pNode ) )
//   {
//  
//   
//     dispatchPost ( pNode );
//   }
}

void ddOgl::dispatch ( geom const* pNode )
{
PNIDBG
  if ( dispatchPre ( pNode ) )
  {
PNIDBG
    geomData const* pData = pNode->getGeomData();
    // Intel driver doesn't like empty indices with bad ptrs. :)
    geomData::Indices const& indices = pData->getIndices ();
    if ( pData && ( ! indices.empty () ) )
    {
      ddOglList::renderItem& item = mDdList.alloc ();
      item.mMatrix = *mMatStack;
      item.mStateSet = mStateStack.back ();
      item.mNode = pNode;
      item.distSqr = calcDistSqr ( pNode );
    }
          
    dispatchPost ( pNode );
  }
}

void ddOgl::dispatch ( geomFx const* pNode )
{
    // TODO Create and dispatch graphDd::fxUpdate to 
    // pNode->updateFx ...
    
  //fxUpdate update;
  //update.mCamPath = mSinkPath;
  //update.mNodePath = mNodePath;
  //update.mCamPos = mCamPos;
    //// Non-optimal... this is redundantly calculated in dispatch for geom.
  //update.mDistanceSqr = calcDistSqr ( pNode );
  //update.mCam = static_cast< camera* > ( mSinkPath.getLeaf () );
  
  fxUpdate update (
    mTimeStamp,
    mTimeStamp - mLastTimeStamp,
    calcDistSqr ( pNode ),
    mCamPos,
    mNodePath,
    mSinkPath,
    static_cast< camera* > ( mSinkPath.getLeaf () )
  );
  
  
    // Must cast away const here... the rationalization is that
    // anything that happens here is affecting the geom as a 
    // side-effect of the const traversal.  Meh.
  geomFx* pGeomFx = const_cast< geomFx* > ( pNode );
  pGeomFx->update ( update );
  
  geom const* pGeom = pNode;
  dispatch ( pGeom );
}

void ddOgl::dispatch ( group const* pNode )
{
PNIDBG

static pni::pstd::timerProf pre ( "dispatchPre", 120 * 5 );
PSTDPROFCALL(pre.start ());

  if ( dispatchPre ( pNode ) )
  {
PSTDPROFCALL(pre.stop ());

    dispatchChildren ( pNode );
  
    dispatchPost ( pNode );
  }
}

void ddOgl::dispatch ( light const* pNode )
{
PNIDBG
//   if ( dispatchPre ( pNode ) )
//   {
//   
//     dispatchPost ( pNode );
//   }
}

/////////////////////////////////////////////////////////////////////

void ddOgl::dispatch ( sndEffect const* pNode )
{
PNIDBG
  // TODO
}

/////////////////////////////////////////////////////////////////////

void ddOgl::dispatch ( sndListener const* pNode )
{
PNIDBG
  // TODO
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// State methods.

void ddOgl::setDefaultState ( state const* pState, state::Id id )
{
  // FIXME Nees better error handling.
  if ( mStateStack.size () != 1 )
    return;

  StdStateStack::value_type& top = mStateStack.back ();
  top.mStates[ id ] = pState;
}

void ddOgl::startStates ( node const* pNode )
{
  // Nothing here because this particular implementation will
  // not evaluate states until they have been put in the sorted
  // render list in ddOglList.
}

void ddOgl::dispatch ( blend const* pState )
{
}

void ddOgl::dispatch ( cull const* pState )
{

}

void ddOgl::dispatch ( depth const* pState )
{

}

void ddOgl::dispatch ( lighting const* pState )
{

}

void ddOgl::dispatch ( lightPath const* pState )
{

}

void ddOgl::dispatch ( material const* pState )
{

}

// void ddOgl::dispatch ( polygonMode const* pState )
// {
// 
// }

void ddOgl::dispatch ( prog const* pState )
{

}

void ddOgl::dispatch ( texEnv const* pState )
{

}

// void ddOgl::dispatch ( texGen const* pState )
// {
// 
// }

void ddOgl::dispatch ( texture const* pState )
{

}

void ddOgl::dispatch ( textureXform const* pState )
{

}
  
void ddOgl::dispatch ( uniform const* pState )
{
  
}

void ddOgl::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  mNodePath.collectRefs(refs);
}
/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


