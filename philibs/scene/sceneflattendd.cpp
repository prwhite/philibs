/////////////////////////////////////////////////////////////////////
//
//    file: sceneflattendd.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneflattendd.h"

#include "scenegroup.h"
#include "scenecamera.h"
#include "scenelight.h"
#include "scenegeom.h"
#include "scenegeomfx.h"
#include "scenesndeffect.h"
#include "scenesndlistener.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

flattenDd::flattenDd () :
  mOpts ( None ),
  mLevel ( 0 )
{
  // TODO
}

//flattenDd::~flattenDd ()
//{
//  // TODO
//}
//
//flattenDd::flattenDd ( flattenDd const& rhs )
//{
//  // TODO
//}
//
//flattenDd& flattenDd::operator= ( flattenDd const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool flattenDd::operator== ( flattenDd const& rhs ) const
//{
//  // TODO
//  return false;
//}

/////////////////////////////////////////////////////////////////////
//  overrides from scene::graphDd

void flattenDd::dispatchChildren ( node const* pNode )
{
  typedef scene::node::Nodes::const_iterator Iter;
  Iter end = pNode->getChildren ().end ();
  for ( Iter cur = pNode->getChildren ().begin (); cur != end; ++cur )
  {
    ( *cur )->accept ( this );
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void flattenDd::startGraph ( node const* pNode )
{
  mLevel = 0;

  mMatStack.reset ();
  
  pni::math::vec3 center;
  pNode->getBounds ().getCenter ( center );
  
  center.xformPt4 ( center, pNode->getMatrix () );
  
  node* const pNcNode = const_cast< node* > ( pNode );
  
  if ( mOpts & ( LeaveCenterAtOrigin | LeaveCenterXform ) )
  {
    pni::math::vec3 invTrans = center;
    invTrans.negate ();
    pNcNode->matrixOp ().postTrans ( invTrans );
  }
  
  pNode->accept ( this );
  
  if ( mOpts & LeaveCenterXform )
    pNcNode->matrixOp ().setTrans ( center );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void flattenDd::dispatchPre ( node const* pNode )
{
  ++mLevel;

  mMatStack.push ();
  
  if ( mMatStack.getCurDepth () > 1 )
    mMatStack->preMult ( pNode->getMatrix () );
  else
    *mMatStack = pNode->getMatrix ();
}

/////////////////////////////////////////////////////////////////////

void flattenDd::dispatchPost ( node const* pNode )
{
    // Now kill the old transform.
  node* const pNcNode = const_cast< node* > ( pNode );
  pNcNode->matrixOp ().setIdentity ();

  mMatStack.pop ();

  --mLevel;  
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void flattenDd::dispatch ( camera const* pNode )
{
  // TODO: Um... what do we do with this?
}

/////////////////////////////////////////////////////////////////////

void flattenDd::dispatch ( geom const* pNode )
{
  dispatchPre ( pNode );

  geom* pGeom = const_cast< geom* > ( pNode );
  geomData* pData = &pGeom->getGeomData()->op();
  
  if ( ! pData->getIndices().empty() )
  {
    pni::math::matrix4 invMat = *mMatStack;
    invMat.invert ();
    invMat.transpose ();

    auto pIter = pData->begin<pni::math::vec3>(geomData::Position);
    auto nIter = pData->begin<pni::math::vec3>(geomData::Normal);
    auto pIterEnd = pData->end<pni::math::vec3>(geomData::Position);
    
    for ( ; pIter != pIterEnd; ++pIter, ++nIter )
    {
      pIter->xformPt4( *pIter, *mMatStack );
      nIter->xformVec4( *nIter, invMat );

      if ( mOpts & Renormalize )
        nIter->normalize ();
    }
  }

  dispatchPost ( pNode );
}

/////////////////////////////////////////////////////////////////////

void flattenDd::dispatch ( geomFx const* pNode )
{
    // Somewhat controversial?  Kinda depends on the
    // geomFx node... but right now we only have geomFx
    // that don't need to be flattened.
  //dispatch ( ( geom* ) pNode );
}

/////////////////////////////////////////////////////////////////////

void flattenDd::dispatch ( group const* pNode )
{
  dispatchPre ( pNode );
  
  dispatchChildren ( pNode );

  dispatchPost ( pNode );
}

/////////////////////////////////////////////////////////////////////

void flattenDd::dispatch ( light const* pNode )
{
  // TODO: This should update position and direction vector.
}

/////////////////////////////////////////////////////////////////////

void flattenDd::dispatch ( sndEffect const* pNode )
{
  sndEffect* const pNcNode = const_cast< sndEffect* > ( pNode );
  
  dispatchPre ( pNode );
  
  pni::math::matrix4 const& mat = *mMatStack;
  
  pni::math::vec3 pos = pNode->getPos ();
  pos.xformPt4 ( pos, mat );
  pNcNode->setPos ( pos );

  dispatchPost ( pNode );
}

/////////////////////////////////////////////////////////////////////

void flattenDd::dispatch ( sndListener const* pNode )
{
  sndListener* const pNcNode = const_cast< sndListener* > ( pNode );
  
  dispatchPre ( pNode );
  
  pni::math::matrix4 const& mat = *mMatStack;
  
  pni::math::vec3 pos = pNode->getPos ();
  pos.xformPt4 ( pos, mat );
  pNcNode->setPos ( pos );

  dispatchPost ( pNode );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


