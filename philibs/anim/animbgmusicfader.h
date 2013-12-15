/////////////////////////////////////////////////////////////////////
//
//  class: bgMusicFader
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimbgmusicfader_h
#define animanimbgmusicfader_h

/////////////////////////////////////////////////////////////////////

#include "animbase.h"

#include "sndengine.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

class bgMusicFader :
  public base
{
  public:
    bgMusicFader ();

    void setEngine ( snd::engine* val ) { mEngine = val; }
    snd::engine* getEngine () const { return mEngine.get (); }
        
  protected:
    virtual ~bgMusicFader ();
    
    typedef pni::pstd::autoRef< snd::engine > SndEngineRef;
    
    SndEngineRef mEngine;
    
  private:


    // interface from base
  public:

  protected:
		
		virtual void collectRefs ( Refs& refs )
		    {
		      refs.push_back ( mEngine.get () );
		    }

};

/////////////////////////////////////////////////////////////////////

class bgMusicFadeIn :
  public bgMusicFader
{
  public:
    bgMusicFadeIn () : mDstLevel ( 1.0f ) {}
  
    void setDstLevel ( float val ) { mDstLevel = val; }
    
  protected:
    virtual void internalUpdate ( manager* pManager, TimeType tval );
    
    float mDstLevel;
};

/////////////////////////////////////////////////////////////////////

class bgMusicFadeOut :
  public bgMusicFader
{
  public:
    bgMusicFadeOut () : mSrcLevel ( 1.0f ) {}
  
    void setSrcLevel ( float val ) { mSrcLevel = val; }
    
  protected:
    virtual void internalUpdate ( manager* pManager, TimeType tval );
    
    float mSrcLevel;
};


/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimbgmusicfader_h


