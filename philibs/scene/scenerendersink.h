/////////////////////////////////////////////////////////////////////
//
//    class: renderSink
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenerendersink_h
#define scenescenerendersink_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "scenetypes.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

  /**
    The renderSink will encapsulate all of the settings related to
    a rendering pass.  This includes things like the graphDd for drawing
    and [optionally] collision and sound, the destination fbo, the traversal
    masks for each graphDd, and a number of other associated settings.  
    Instances of this class also form a graph of their own, which implies the
    dependency graph for rendering order of all renderSinks.
    @note This class is not implemented yet.
  */
class renderSink :
  public pni::pstd::refCount,
  public scene::travDataContainer
{
  public:
    renderSink() = default;
//    virtual ~renderSink();
    renderSink(renderSink const& rhs) = delete;
    renderSink& operator=(renderSink const& rhs) = delete;
//    bool operator==(renderSink const& rhs) const;
  
  protected:
      
  private:
  

	// interface from pni::pstd::refCount and scene::travDataContainer
	public:
    void collectRefs ( pni::pstd::refCount::Refs& refs );
  
	protected:
		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenerendersink_h


