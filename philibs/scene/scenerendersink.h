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
#include "scenegraphdd.h"
#include "scenenode.h"
#include "sceneisectdd.h"
#include "sceneframebuffer.h"

#include <algorithm>

/////////////////////////////////////////////////////////////////////

namespace scene {

class renderSinkDd;

/////////////////////////////////////////////////////////////////////

  /**
    The renderSink encapsulates all of the settings related to
    a rendering pass.  This includes things like the graphDd for drawing
    and [optionally] collision and sound, the destination fbo, the traversal
    masks for each graphDd, and a number of other associated settings.  
    Instances of this class also form a graph of their own, which implies the
    dependency graph for rendering order of all renderSinks.
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

    using ChildRef = pni::pstd::autoRef<renderSink>;
    using Children = std::vector< ChildRef >;

      /// @group Manage child renderSinks
  
      /// There are no parent pointers.
    void addChild ( renderSink* pChild ) { mChildren.push_back ( pChild ); }
      ///
    void remChild ( renderSink* pChild )
      {
        auto found = std::find(mChildren.begin(), mChildren.end(), pChild);
        if (found != mChildren.end ())
          mChildren.erase(found);
      }
      ///
    void remChild ( size_t which )
      {
        auto iter = mChildren.begin();
        iter += which;
        mChildren.erase(iter);
      }
      ///
    renderSink* getChild ( size_t which ) const { return mChildren[ which ].get(); }
  
      /// Get a const version of the child list.
    Children const& getChildren () const { return mChildren; }

      // First part of double dispatch with renderSinkDd.
    virtual void accept ( renderSinkDd* pDd ) const;
  
      /// @group Manage data members for this render sink.
  
    typedef pni::pstd::autoRef<graphDd> GraphDdRef;
    typedef pni::pstd::autoRef<isectDd> IsectDdRef;
    typedef pni::pstd::autoRef<isectTest> IsectTestRef;
    typedef pni::pstd::autoRef<framebuffer> FramebufferRef;
    typedef pni::pstd::autoRef<node> NodeRef;
    typedef node::MaskType MaskType;

      // Not a lot of side-effects anticipated for these members,
      // and they're already memory-managed.  So public!!!
  
      /// Specify the frame buffer parameters for this sink.
    FramebufferRef mFramebuffer;
  
      /// It is ok for the mDd to be a nullptr.  This indicates to
      /// the system to skip a given rendering pass.
    struct graphDdSpec
    {
      GraphDdRef mDd;                  // = nullptr
      scene::nodePath mSinkPath;       // = nullptr
      NodeRef mRootNode;               // = nullptr
      MaskType mTravMask                  = TravMaskDefaultDraw;
    };

      /// Specify the drawing parameters for this sink
    graphDdSpec mDrawSpec;
      /// Specify the sound parameters for this sink
    graphDdSpec mSndSpec;
      /// Specify the instersection parameters for this sink
    graphDdSpec mIsectSpec;
  
      /// Different renderSink @a can use different textures on aliases to
      /// the same framebuffer... although that will cause some state thrash.
    framebuffer::textureTargets mTextureTargets;

  protected:

  private:
    Children mChildren;

	// interface from pni::pstd::refCount and scene::travDataContainer
	public:
    void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
  
	protected:
		

};

/* Reference from ui::layer... a very similar concept...

    scene::graphDd* mDd;
    scene::graphDd* mSndDd;
    scene::nodePath mCamPath;
    scene::nodePath mSndListenerPath;
    unsigned int mTravMask;
    unsigned int mSndTravMask;
    FrameRef mRoot;
    pni::pstd::autoRef< scene::node > mSndRoot;
    unsigned int mOps;
  
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
*/


/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenerendersink_h


