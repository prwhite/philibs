/////////////////////////////////////////////////////////////////////
//
//    file: animuibasetrans.cpp
//
/////////////////////////////////////////////////////////////////////

#include "animuibasetrans.h"

// Temporary:
#include "pnimathstream.h"
#include <iostream>

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

// uiBaseTrans::uiBaseTrans()
// {
//     
// }

// uiBaseTrans::~uiBaseTrans()
// {
// PNIDBG
// }

// uiBaseTrans::uiBaseTrans(uiBaseTrans const& rhs)
// {
//     
// }
// 
// uiBaseTrans& uiBaseTrans::operator=(uiBaseTrans const& rhs)
// {
//     
//     return *this;
// }
// 
// bool uiBaseTrans::operator==(uiBaseTrans const& rhs) const
// {
//     
//     return false;
// }

/////////////////////////////////////////////////////////////////////
// overrides from anim::base

void uiBaseTrans::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  trans::collectRefs ( refs );
  refs.push_back ( mUiBase.get () );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace anim 


