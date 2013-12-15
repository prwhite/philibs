/////////////////////////////////////////////////////////////////////
//
//    file: scenelightpath.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenelightpath.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

lightPath::lightPath()
{
  mNodePaths.resize ( MaxNodePaths );
  
  //NodePaths::iterator end = mNodePaths.end ();
  //for ( NodePaths::iterator cur = mNodePaths.begin ();
  //    cur != end; ++cur )
  //{
  //  cur->setDoRef ( false );
  //}
}

// lightPath::~lightPath()
// {
//   
// }

// lightPath::lightPath(lightPath const& rhs)
// {
//   
// }
// 
// lightPath& lightPath::operator=(lightPath const& rhs)
// {
//   
//   return *this;
// }
// 
// bool lightPath::operator==(lightPath const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from state


void lightPath::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  state::collectRefs ( refs );

  NodePaths::iterator end = mNodePaths.end ();
  for ( NodePaths::iterator cur = mNodePaths.begin (); cur != end; ++cur )
    cur->collectRefs ( refs );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


