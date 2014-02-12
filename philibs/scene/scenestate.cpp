/////////////////////////////////////////////////////////////////////
//
//    file: scenestate.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenestate.h"

#include "pnirefcountdbg.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

state::state() :
  mOverride ( false ),
  mEnable ( true )
{

}

state::~state()
{
  
}

// state::state(state const& rhs) :
//   mOverride ( rhs.mOverride )
// {
//   
// }

//state& state::operator=(state const& rhs)
//{
//    
//    return *this;
//}

// bool state::operator==(state const& rhs) const
// {
//   // What else will constitute equality for states...
//   // or are we just using it for type equality?
//   // Blend state is an important one... so that
//   // we sort transparent stuff after opaque... but
//   // really that will come through a custom "op <".
//   if ( getId () == rhs.getId () )
//     return true;
//   else
//     return false;
// }

/////////////////////////////////////////////////////////////////////

void state::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  pni::pstd::dbgRefCount::collectMapSecondRefs ( getTravDatum (), refs );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


