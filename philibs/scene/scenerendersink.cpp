/////////////////////////////////////////////////////////////////////
//
//    file: scenerendersink.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenerendersink.h"
#include "pnirefcountdbg.h"
#include "scenerendersinkdd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

//renderSink::renderSink()
//{
//    
//}
//
//renderSink::~renderSink()
//{
//    
//}
//
//renderSink::renderSink(renderSink const& rhs)
//{
//    
//}
//
//renderSink& renderSink::operator=(renderSink const& rhs)
//{
//    
//    return *this;
//}
//
//bool renderSink::operator==(renderSink const& rhs) const
//{
//    
//    return false;
//}

void renderSink::accept ( renderSinkDd* pDd ) const
{
  pDd->dispatch ( this );
}


/////////////////////////////////////////////////////////////////////
// overrides from pni::pstd::travDataContainer

void renderSink::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  pni::pstd::dbgRefCount::collectMapSecondRefs ( getTravDatum (), refs );
  mTextureTargets.collectRefs(refs);
}


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


