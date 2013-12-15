/////////////////////////////////////////////////////////////////////
//
//  class: sndListener
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenesndlistener_h
#define scenescenesndlistener_h

/////////////////////////////////////////////////////////////////////

#include "scenenode.h"
#include "scenegraphdd.h"

#include "sndengine.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class sndListener :
  public node
{
  public:
    typedef snd::ErrorType ErrorType;
  
    sndListener ();
    //sndListener ( sndListener const& rhs );
    //sndListener& operator= ( sndListener const& rhs );
    //bool operator== ( sndListener const& rhs ) const;

      // Whoa... this is not what I thought it was.  In SoundEngine.cpp
      // this iterates over all current sounds and sets their volume to
      // this value... hardly ever a useful thing.
    //void setEffectsVolume ( float val ) { mEffectsVolume = val; }
    //float getEffectsVolume () const { return mEffectsVolume; }

    void setListenerGain ( float val ) { mGain = val; }
    float getListenerGain () const { return mGain; }

    void setMaxDist ( float val ) 
        { 
          mMaxDist = val;
        }
    float getMaxDist () const { return mMaxDist; }

    void setRefDist ( float val ) 
        { 
          mRefDist = val;
        }
    float getRefDist () const { return mRefDist; }

    void setPos ( pni::math::vec3 const& val ) { mPos = val; }
    pni::math::vec3 const& getPos () const { return mPos; }

  protected:
    virtual ~sndListener ();

    float mEffectsVolume;
    float mGain;
    float mMaxDist;
    float mRefDist;
    pni::math::vec3 mPos;

  private:


    // interface from node
  public:
  
    virtual node* dup () const { return new sndListener ( *this ); }
    virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }

  protected:
    virtual void collectRefs ( Refs& refs )
        {
          node::collectRefs ( refs );
        }		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenesndlistener_h


