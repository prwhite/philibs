/////////////////////////////////////////////////////////////////////
//
//  class: sndDd
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenesnddd_h
#define scenescenesnddd_h

/////////////////////////////////////////////////////////////////////

#include "scenegraphdd.h"
#include "sndengine.h"

#include "pnimatstack.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class sndDd :
  public graphDd
{
  public:
    sndDd ();
    virtual ~sndDd ();
    //sndDd ( sndDd const& rhs );
    //sndDd& operator= ( sndDd const& rhs );
    //bool operator== ( sndDd const& rhs ) const;
    
    void setEngine ( snd::engine* val ) { mEngine = val; }
    snd::engine* getEngine () const { return mEngine.get (); }
    
  protected:

  private:


    // interface from graphDd
  public:
    virtual void startGraph ( node const* pNode );

  protected:
    void preNode ( node const* pNode );
    void postNode ( node const* pNode );
    void visitNode ( node const* pNode );
    void visitChildren ( node const* pNode );
    void applyListener ();

    typedef pni::pstd::autoRef< snd::engine > SndEngineRef;
  
    SndEngineRef mEngine;
    pni::math::matStack< pni::math::matrix4 > mMatStack;
    pni::math::vec3 mListenerPos;
  
    virtual void dispatch ( camera const* pNode );
    virtual void dispatch ( geom const* pNode );
    virtual void dispatch ( group const* pNode );
    virtual void dispatch ( light const* pNode );
    virtual void dispatch ( geomFx const* pNode );
    virtual void dispatch ( sndEffect const* pNode );
    virtual void dispatch ( sndListener const* pNode );
		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenesnddd_h


