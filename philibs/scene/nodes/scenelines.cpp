// ////////////////////////////////////////////////////////////////////////////
//
//  class: lines
//
// ////////////////////////////////////////////////////////////////////////////

#include "scenelines.h"
#include "pnivec3.h"
#include "pnivec4.h"

#include <cassert>

// ///////////////////////////////////////////////////////////////////

using namespace pni::math;

namespace scene {
    
// ///////////////////////////////////////////////////////////////////


void lines::update ( graphDd::fxUpdate const& update )
{
  assert(mLineData);
  assert(mLineData->mBinding.hasBinding(lineData::Position));
  assert(mLineData->size());

  auto curPos = mLineData->begin(lineData::Position);
  auto curColor = mLineData->begin(lineData::Color);
  auto curThickness = mLineData->begin(lineData::Thickness);

    // Iterate through all line sigments
  for ( auto segNum : mLineData->getIndices() )
  {
    size_t end = segNum - 1;  // we are evaluating points in pairs, so stop one early

      // Iterate through all points in each segment
    for ( size_t cur = 0; cur < end; ++cur )
    {
        // Position
      vec3* curPosVec = curPos;
      ++curPos;
      vec3* nextPosVec = curPos;
      
      if ( curColor.good () )
      {
        vec4* curColorVec = curColor;
        ++curColor;
        vec4* nextColorVec = curColor;
      }
      
      if ( curThickness.good () )
      {
        float* curThicknessFloat = curThickness;
        ++curThickness;
        float* nextThicknessFloat = curThickness;
      }
    }
    
    ++curPos;
    if ( curColor.good () ) ++curColor;
    if ( curThickness.good () ) ++curThickness;
  }
  
}


// ///////////////////////////////////////////////////////////////////


} // end of namespace scene 

