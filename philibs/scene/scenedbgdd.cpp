/////////////////////////////////////////////////////////////////////
//
//    file: scenedbgdd.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenedbgdd.h"

#include "pnimathstream.h"

#include "scenecamera.h"
#include "scenegeom.h"
#include "scenegeomfx.h"
#include "scenegroup.h"
#include "scenelight.h"
#include "sceneprog.h"
#include "scenetexture.h"
#include "sceneuniform.h"
#include "scenesndeffect.h"
#include "scenesndlistener.h"


/////////////////////////////////////////////////////////////////////

using namespace std;

namespace scene {
  
/////////////////////////////////////////////////////////////////////

dbgDd::dbgDd ( std::ostream& str ) :
  mIndent ( 0 ),
  mOpts ( Normal ),
  mStr ( str )
{
  
}

//dbgDd::~dbgDd()
//{
//    
//}
//
//dbgDd::dbgDd(dbgDd const& rhs)
//{
//    
//}
//
//dbgDd& dbgDd::operator=(dbgDd const& rhs)
//{
//    
//    return *this;
//}
//
//bool dbgDd::operator==(dbgDd const& rhs) const
//{
//    
//    return false;
//}

/////////////////////////////////////////////////////////////////////
// overrides from graphDd

//void dbgDd::indent ( std::string const& str )
//{
//  for ( size_t num = 0; num < mIndent; ++num )
//    mStr << str;
//}

std::ostream& dbgDd::indent ( std::string const& str )
{
  for ( size_t num = 0; num < mIndent; ++num )
    mStr << str;
    
  return mStr;
}

void dbgDd::startGraph ( node const* pNode )
{
  mIndent = 0;
  mTexSet.clear ();
  mGeomDataSet.clear ();
  
  pNode->accept ( this );
  
  doTotals ();
}

void dbgDd::doTotals ()
{
  size_t geomDataMemTotal = 0;;
  
  typedef GeomDataSet::iterator Giter;
  for ( Giter cur = mGeomDataSet.begin (); 
      cur != mGeomDataSet.end ();
      ++cur )
  {
    if ( scene::geomData const* pData = *cur )
    {
      geomDataMemTotal += pData->getValues ().size () * 
          sizeof ( float );
      geomDataMemTotal += pData->getIndices ().size () * 
          sizeof( geomData::SizeType );
    }
  }

  size_t textureMemTotal = 0;
  typedef TexSet::iterator Titer;
  for ( Titer cur = mTexSet.begin ();
      cur != mTexSet.end ();
      ++cur )
  {
    texture const* pState = *cur;
    if ( img::base* pImg = pState->getImage () )
    {
      for ( size_t num = 0; num < pImg->mBuffers.size (); ++num )
      {
        textureMemTotal += pImg->mBuffers[ num ]->size ();
      }
    }
  }

  mStr << "Total Texture Memory Used: " << textureMemTotal << std::endl;
  mStr << "Total GeomData Memory Used: " << geomDataMemTotal << std::endl;

  mTexSet.clear ();
  mGeomDataSet.clear ();  
}

void dbgDd::dispatchChildren ( node const* pNode )
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

void dbgDd::dispatchTravData ( node const* pNode )
{
  if ( ! ( mOpts & TravData  ) )
    return;

  indent () << "travData = ";
  
  for ( int num = 0; num < scene::TravCount; ++num )
  {
    if ( pNode->getTravData ( static_cast< scene::Trav > ( num ) ) )
      mStr << "1";
    else
      mStr << "0";
  }
  
  mStr << endl;
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatchXform ( node const* pNode )
{
  if ( ! ( mOpts & Xform ) )
    return;
  
  ++mIndent;
    
  pni::math::matrix4 const& mat = pNode->getMatrix ();
  pni::math::vec3 tmp;
  
  mat.getTrans ( tmp );
  indent () << "trans = " << tmp << endl;

  pni::math::matrix4 tmpMat;
  mat.getRot ( tmpMat );
  tmpMat.getRow ( 0, tmp );
  indent () << "rot mat = " << tmp << endl;
  tmpMat.getRow ( 1, tmp );
  indent () << "          " << tmp << endl;
  tmpMat.getRow ( 2, tmp );
  indent () << "          " << tmp << endl;
  
  mat.getScale ( tmp );
  indent () << "scale = " << tmp << endl;
  
  --mIndent;
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatchBounds ( node const* pNode )
{
  if ( ! ( mOpts & Bounds ) )
    return;
  
  ++mIndent;
  
  pni::math::box3 bounds = pNode->getBounds ();
  pni::math::vec3 minVal, maxVal;
  bounds.get ( minVal, maxVal );
  
  indent () << "local bounds = " << endl;
  indent () << "  min = " << minVal << endl;
  indent () << "  max = " << maxVal << endl;
  
  bounds.xform4 ( bounds, pNode->getMatrix () );
  bounds.get ( minVal, maxVal );
  
  indent () << "parent bounds = " << endl;
  indent () << "  min = " << minVal << endl;
  indent () << "  max = " << maxVal << endl;
  
  --mIndent;
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatchNode ( node const* pNode )
{
  indent (); mStr << "node name = " << pNode->getName () << 
      " refs = " << pNode->getNumRefs () << endl;
  dispatchXform ( pNode );
  dispatchBounds ( pNode );
  startStates ( pNode );
  dispatchTravData ( pNode );
  dispatchTextureMem ( pNode );
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatchTextureMem ( node const* pNode )
{
  if ( scene::state const* pState = pNode->getState ( scene::state::Texture0 ) )
    pState->accept ( this );    

  if ( scene::state const* pState = pNode->getState ( scene::state::Texture1 ) )
    pState->accept ( this );
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatch ( camera const* pNode )
{
  indent ( ". " ); mStr << "node type = camera" << endl;
  ++mIndent;
  dispatchNode ( pNode );
  --mIndent;
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatchGeom ( geom const* pNode )
{
  dispatchNode ( pNode );

  if ( pNode->getGeomData () )
  {
    mGeomDataSet.insert ( pNode->getGeomData () );

      // Holy shit this will be a lot of output...
    if ( mOpts & GeomDataValues )
      pNode->getGeomData()->dbg( mStr );
  }
}


void dbgDd::dispatch ( geom const* pNode )
{
  indent ( ". " ); mStr << "node type = geom" << endl;
  ++mIndent;
  dispatchGeom ( pNode );
  --mIndent;
}

void dbgDd::dispatch ( geomFx const* pNode )
{
  indent ( ". " ); mStr << "node type = geomFx" << endl;
  ++mIndent;
  dispatchGeom ( pNode );
  --mIndent;
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatch ( sndEffect const* pNode )
{
  indent ( ". " ); mStr << "node type = sndEffect" << endl;
  ++mIndent;
  dispatchNode ( pNode );
  --mIndent;
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatch ( sndListener const* pNode )
{
  indent ( ". " ); mStr << "node type = sndListener" << endl;
  ++mIndent;
  dispatchNode ( pNode );
  --mIndent;
  // TODO
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatch ( group const* pNode )
{
  indent ( ". " ); mStr << "node type = group" << endl;
  ++mIndent;
  dispatchNode ( pNode );
  dispatchChildren ( pNode );
  --mIndent;
}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatch ( light const* pNode )
{
  indent ( ". " ); mStr << "node type = light" << endl;
  ++mIndent;
  dispatchNode ( pNode );
  --mIndent;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void dbgDd::startStates ( node const* pNode )
{
  if ( ! ( mOpts & States ) )
    return;

  indent ( "  " ); mStr << "states" << endl;
  ++mIndent;

  typedef node::States::const_iterator Iter;
  
  Iter end = pNode->getStates ().end ();
  for ( Iter cur = pNode->getStates ().begin (); cur != end; ++cur )
    cur->second->accept ( this );
    
  --mIndent;
}

void dbgDd::setDefaultState ( state const* pState, state::Id id )
{

}

/////////////////////////////////////////////////////////////////////

void dbgDd::dispatch ( blend const* pState )
{
  indent ( "  " ); mStr << "state type = blend" << endl;
}

void dbgDd::dispatch ( cull const* pState )
{
  indent ( "  " ); mStr << "state type = cull" << endl;
}

void dbgDd::dispatch ( depth const* pState )
{
  indent ( "  " ); mStr << "state type = depth" << endl;
}

void dbgDd::dispatch ( lighting const* pState )
{
  indent ( "  " ); mStr << "state type = lighting" << endl;
}

void dbgDd::dispatch ( lightPath const* pState )
{
  indent ( "  " ); mStr << "state type = lightPath" << endl;
}

void dbgDd::dispatch ( material const* pState )
{
  indent ( "  " ); mStr << "state type = material" << endl;
}

void dbgDd::dispatch ( prog const* pState )
{
  indent ( "  " ); mStr << "state type = prog" << endl;
}

void dbgDd::dispatch ( texEnv const* pState )
{
  indent ( "  " ); mStr << "state type = texEnv" << endl;
}

void dbgDd::dispatch ( texture const* pState )
{
    // HACK... OMFG... using the comma operator below
    // to avoid having extra squigly braces.
  if ( mOpts & States )
    indent ( "  " ), mStr << "state type = texture" << endl;

  mTexSet.insert ( pState );  
}

void dbgDd::dispatch ( textureXform const* pState )
{
  indent ( "  " ); mStr << "state type = textureXform" << endl;
}

void dbgDd::dispatch ( uniform const* pState )
{
  indent ( "  " ); mStr << "state type = uniform" << endl;
}



} // end of namespace scene 


