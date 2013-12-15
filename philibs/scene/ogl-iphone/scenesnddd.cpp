/////////////////////////////////////////////////////////////////////
//
//    file: scenesnddd.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenesnddd.h"

#include "scenegroup.h"
#include "scenesndlistener.h"
#include "scenesndeffect.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

sndDd::sndDd ()
{
  // TODO
}

sndDd::~sndDd ()
{
  // TODO
}

//sndDd::sndDd ( sndDd const& rhs )
//{
//  // TODO
//}
//
//sndDd& sndDd::operator= ( sndDd const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool sndDd::operator== ( sndDd const& rhs ) const
//{
//  // TODO
//  return false;
//}

/////////////////////////////////////////////////////////////////////

void sndDd::preNode ( node const* pNode )
{
  scene::matrix4 const& mat = pNode->getMatrix ();
  
  if ( ! mat.isIdentity () )
  {
    mMatStack.push ();
    mMatStack->preMult ( mat );
  }
}

/////////////////////////////////////////////////////////////////////

void sndDd::visitNode ( node const* pNode )
{
  
}

/////////////////////////////////////////////////////////////////////

void sndDd::postNode ( node const* pNode )
{
  scene::matrix4 const& mat = pNode->getMatrix ();
  
  if ( ! mat.isIdentity () )
  {
    mMatStack.pop ();
  }
}

/////////////////////////////////////////////////////////////////////

void sndDd::visitChildren ( node const* pNode )
{
  node::Nodes const& kids = pNode->getChildren ();  
  typedef node::Nodes::const_iterator ConstIter;

  ConstIter end = kids.end ();
  for ( ConstIter cur = kids.begin (); cur != end; ++cur )
  {
    if ( ( *cur )->getTravMask ( Sound ) & mTravMask )
      ( *cur )->accept ( this );
  }
}

/////////////////////////////////////////////////////////////////////

void sndDd::applyListener ()
{
  if ( mSinkPath.empty () )
    return;
    
  sndListener* pListener = static_cast< sndListener* > (
      mSinkPath.getLeaf () );
      
  mListenerPos = pListener->getPos ();
  
  pni::math::matrix4 mat ( pni::math::matrix4::NoInit );
  mSinkPath.calcXform ( mat );
  
  mListenerPos.xformPt4 ( mListenerPos, mat );
  
  mEngine->setListenerPos ( mListenerPos );
  //mEngine->setEffectsVolume ( pListener->getEffectsVolume () );
  mEngine->setMaxDist ( pListener->getMaxDist () );
  mEngine->setRefDist ( pListener->getRefDist () );
  mEngine->setListenerGain ( pListener->getListenerGain () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
  //  overrides from graphDd

void sndDd::startGraph ( node const* pNode )
{
  mMatStack.push ();
  mMatStack->setIdentity ();
  
  applyListener ();
  
  if ( pNode->getTravMask ( Sound ) & mTravMask )
    pNode->accept ( this );
  
  mMatStack.pop ();
}

/////////////////////////////////////////////////////////////////////

void sndDd::dispatch ( camera const* pNode )
{
  // TODO
}

/////////////////////////////////////////////////////////////////////

void sndDd::dispatch ( geom const* pNode )
{
  // TODO
}

/////////////////////////////////////////////////////////////////////

void sndDd::dispatch ( group const* pNode )
{
  preNode ( pNode );
  
  visitChildren ( pNode );
  
  postNode ( pNode );
}

/////////////////////////////////////////////////////////////////////

void sndDd::dispatch ( light const* pNode )
{
  // TODO
}

/////////////////////////////////////////////////////////////////////

void sndDd::dispatch ( geomFx const* pNode )
{
  // TODO
}

/////////////////////////////////////////////////////////////////////

void sndDd::dispatch ( sndEffect const* pNode )
{
  preNode ( pNode );

  pni::math::vec3 pos = pNode->getPos ();
  
  pos.xformPt4 ( pos, *mMatStack );

//printf ( " sndDd::dispatch : %x, pos : %f, %f, %f\n",
//    this, pos[ 0 ], pos[ 1 ], pos[ 2 ] );

  //pNode->setEffectPos ( pos );
  pNode->getEffect ()->setPos ( pos );        
  
  postNode ( pNode );
}

/////////////////////////////////////////////////////////////////////

void sndDd::dispatch ( sndListener const* pNode )
{
  // TODO
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


