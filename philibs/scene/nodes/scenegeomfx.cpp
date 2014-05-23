/////////////////////////////////////////////////////////////////////
//
//    file: scenegeomfx.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenegeomfx.h"
#include "sceneconverter.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

geomFx::geomFx ()
{
  // TODO
}

//geomFx::~geomFx ()
//{
//  // TODO
//}
//
//geomFx::geomFx ( geomFx const& rhs )
//{
//  // TODO
//}
//
//geomFx& geomFx::operator= ( geomFx const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool geomFx::operator== ( geomFx const& rhs ) const
//{
//  // TODO
//  return false;
//}


void geomFx::calcCamToWorldRotMat ( graphDd::fxUpdate const& update,
    pni::math::matrix4& mat )
{
    // Get cam -> world matrix to come up with screen-aligned
    // vectors in this node's frame.
    // Invert because the converter gives us world to cam, not
    // cam to world.
  scene::converter conv;
  conv.getWorldToCamMatrix ( mat, update.mCamPath, update.mNodePath );
  mat.invert ();
  
    // Get rid of translate/projection.
    // In some cases losing the projection could be a problem,
    // but in the most likely cases it's no thing at all.
  mat.set3x3Mat ( mat );  // TRICKY: Set to self.  Should be OK.
}

void geomFx::calcXYHalfVecs ( pni::math::vec3& xvec, pni::math::vec3& yvec,
    pni::math::matrix4 const& mat )
{
    // The rows represent the transformed i, j and k vectors.
    // So we don't need to do a mult, we just grab them directly.
  mat.getRow ( 0, xvec );
  mat.getRow ( 1, yvec );
  
    // Kinda normalize them so that they are unit length in the target frame.
    // We make them half unit length because we will be adding them
    // in each direction from the center of the sprite.
  xvec /= 2.0f * xvec.length ();
  yvec /= 2.0f * yvec.length ();
}


/////////////////////////////////////////////////////////////////////
//  overrides from geom


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


