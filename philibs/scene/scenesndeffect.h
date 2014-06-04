/////////////////////////////////////////////////////////////////////
//
//  class: sndEffect
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenesndeffect_h
#define scenescenesndeffect_h

/////////////////////////////////////////////////////////////////////

#include "scenenode.h"
#include "scenegraphdd.h"

#include "sndengine.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class sndEffect :
  public node
{
  public:
    typedef snd::ErrorType ErrorType;
  
    sndEffect ();
    //sndEffect ( sndEffect const& rhs );
    //sndEffect& operator= ( sndEffect const& rhs );
    //bool operator== ( sndEffect const& rhs ) const;
    
    ErrorType setEffect ( snd::effect* pEffect ) 
        { 
          ErrorType errVal = 0;
          mEffect = pEffect; 
          errVal |= setPitch ( mPitch );
          errVal |= setLevel ( mLevel );
          return errVal;
        }
    snd::effect* getEffect () const { return mEffect.get (); }
    
      // OPTIMIZATION: We apply settings to the effect
      // immediately, because effects cannot be instanced
      // in SoundEngine, we don't need to save these updates
      // until the visitor pass.
    ErrorType setPitch ( float val ) 
        { 
          mPitch = val; 
          if ( mEffect )
            return mEffect->setPitch ( mPitch );
          else
            return 0;
        }
    float getPitch () const { return mPitch; }
    
    ErrorType setLevel ( float val ) 
        { 
          mLevel = val;
          if ( mEffect )
            return mEffect->setLevel ( mLevel );
          else
            return 0;
        }
    float getLevel () const { return mLevel; }

    void setPos ( pni::math::vec3 const& val ) { mPos = val; }
    pni::math::vec3 const& getPos () const { return mPos; }
    
    ErrorType start ()
        {
          if ( mEffect )  
            return mEffect->start ();
          else
            return 0;
        }
    ErrorType stop ( bool doEnd = true )
        {
          if ( mEffect )  
            return mEffect->stop ( doEnd );
          else
            return 0;
        }

  protected:
    virtual ~sndEffect ();
    
    typedef pni::pstd::autoRef< snd::effect > EffectRef;
    
    mutable EffectRef mEffect;
    float mPitch;
    float mLevel;
    pni::math::vec3 mPos;
    
  private:


    // interface from node
  public:
    virtual sndEffect* dup () const { return new sndEffect ( *this ); }
    virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }

  protected:
		void collectRefs ( Refs& refs )
		    {
		      node::collectRefs ( refs );
		      refs.push_back ( mEffect.get () );
		    }

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenesndeffect_h


