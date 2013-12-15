/////////////////////////////////////////////////////////////////////
//
//    class: base
//
/////////////////////////////////////////////////////////////////////

#ifndef sceneloaderloaderbase_h
#define sceneloaderloaderbase_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include <string>
#include <map>
#include "scenetexture.h"
#include "sceneblend.h"
#include "pniautoref.h"
#include "pnierror.h"
#include "scenenode.h"
#include "pniprogress.h"

/////////////////////////////////////////////////////////////////////

namespace loader {
  
/////////////////////////////////////////////////////////////////////

class cache :
  public pni::pstd::refCount
{
  public:
    cache () :
      mDefBlend ( new scene::blend )
        {
          mDefBlend->setEnable ( true );
        }
  
    typedef pni::pstd::autoRef< scene::texture > TextureRef;
    typedef std::map< std::string, TextureRef > Textures;
      
    Textures mTextures;
    
    typedef pni::pstd::autoRef< scene::blend > BlendRef;
    BlendRef mDefBlend;
    
  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
};

/////////////////////////////////////////////////////////////////////

class directory :
  public pni::pstd::refCount
{
  public:
    directory () :
      mEnable ( true )
        {}
    
      ///////
      // Nodes
    typedef std::multimap< std::string, pni::pstd::autoRef< scene::node > > Nodes;
    typedef Nodes::iterator NodeIter;
    typedef std::pair< NodeIter, NodeIter > NodeRange;

      // Add a node to the directory.
    void addNode ( scene::node* pNode );

      // Gets first node in dir with name.
    scene::node* getNode ( std::string const& name );
    
      // Gets lower/upper iterators for node name query.
    NodeRange getNodes ( std::string const& name );
    
      ///////
      // Helper methods.
      
      // Initialize directory with all contents of sub-graph.
    void addScene ( scene::node* pNode );
    
      ///////
      // TODO: Materials.
      
      ///////
      // TODO: Textures.
      
      ///////
      // Directory management.
    void clearDir () { mNodes.clear (); }
    void gc ();
    
    void setDirEnable ( bool val ) { mEnable = val; }
    bool getDirEnable () const { return mEnable; }
  
  protected:
    ~directory () {}
    
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
  
  private:
    Nodes mNodes;
    bool mEnable;
};

/////////////////////////////////////////////////////////////////////

class observer
{
  public:
    virtual void onPreNode ( scene::node* pNode ) {}
    virtual void onPostNode ( scene::node* pNode ) {}
    
    virtual void onPreState ( scene::state* pState ) {}
    virtual void onPostState ( scene::state* pState ) {}
    
    virtual void onError ( pni::pstd::error const& err ) {}
};

/////////////////////////////////////////////////////////////////////

enum Errors {
	Info = 1,

	FileNotFound = 100,
	TextureNotFound,
	ResourceNotFound,

	InternalErrorGeom = 1000,
	InternalErrorHierarchy,
	InternalErrorTexture,
	InternalErrorMaterial,
	InternalErrorState,
	InternalErrorXform
};
  
/////////////////////////////////////////////////////////////////////

class base :
  public pni::pstd::refCount,
  public observer,
  public pni::pstd::progress,
  protected pni::pstd::progressObserver
{
  protected:
    virtual ~base();

  public:
    base();
//       base(base const& rhs);
//       base& operator=(base const& rhs);
//       bool operator==(base const& rhs) const;
    
    virtual scene::node* load ( std::string const& fname ) = 0;    

    // TODO: Loader attrs?
    //   Geom ops.
    //   Texture/material/shader cache, etc.
    //   Loading pre/post callbacks.
    //   Application singletons like RTCM manager.
    //   Search paths.
    
    // Temporarily handle cache separately.
    typedef pni::pstd::autoRef< cache > CacheRef;
    void setCache ( cache* pCache ) { mCache = pCache; }
    cache* getCache () const { return mCache.get (); }
    
    // Loader Directory.
    typedef pni::pstd::autoRef< directory > DirRef;
    void setDirectory ( directory* pDir ) { mDirectory = pDir; }
    directory* getDirectory () const { return mDirectory.get (); }
    
    // Loader observer.
      // Never set observer to 0... default is set to base which
      // has protected inheritance from the loader::observer class.
    void setObserver ( observer* pObserver ) { mObserver = pObserver; }
    observer* getObserver () const { return mObserver; }
    
    
  protected:
  
    CacheRef mCache;
    observer* mObserver;
    DirRef mDirectory;
    
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
    
  private:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace loader 

#endif // sceneloaderloaderbase_h


