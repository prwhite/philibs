/////////////////////////////////////////////////////////////////////
//
//  
//
/////////////////////////////////////////////////////////////////////

#ifndef sndengine_h
#define sndengine_h

/////////////////////////////////////////////////////////////////////

#include "pniplatform.h"
#include "pnirefcount.h"

#include "pnivec3.h"
#include <string>

#ifdef PNI_TARGET_IPHONE
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioToolbox/AudioToolbox.h>
#endif // PNI_TARGET_IPHONE

/////////////////////////////////////////////////////////////////////

namespace snd
{

/////////////////////////////////////////////////////////////////////

  typedef unsigned int ErrorType;

/////////////////////////////////////////////////////////////////////

class engine :
  public pni::pstd::refCount
{
  public:
    engine ();
    engine ( float mixerOutputRate );
    
    ErrorType setMasterVolume ( float val );
    ErrorType setListenerPos ( pni::math::vec3 const& pos );
    ErrorType setListenerGain ( float val );
    
    ErrorType loadBgMusicTrack ( std::string const& path, bool addToQueue = true, bool loadAtOnce = false );
    ErrorType unloadBgMusicTrack ();
    ErrorType startBgMusic ();
    ErrorType stopBgMusic ( bool stopAtEnd = false );
    ErrorType setBgMusicVolume ( float val );
    
    ErrorType setEffectsVolume ( float val );
    ErrorType setMaxDist ( float val );
    ErrorType setRefDist ( float val );
    
    ErrorType doVibrate ();
    
  protected:
    virtual ~engine ();
  
    virtual void collectRefs ( Refs& refs )
        {
        
        }
};

/////////////////////////////////////////////////////////////////////

class effect :
  public pni::pstd::refCount
{
  public:
  
    effect ( std::string const& start, 
        std::string const& loop,
        std::string const& end );
    
    effect ( std::string const& fname );
    
    ErrorType start ();
    ErrorType stop ( bool doEnd = true );
    
    ErrorType setPitch ( float val );
    ErrorType setLevel ( float val );
    ErrorType setPos ( pni::math::vec3 const& pos );
  
  protected:
    virtual ~effect ();
    
    virtual void collectRefs ( Refs& refs )
        {
        
        }
    
  
#ifdef PNI_TARGET_IPHONE
    typedef UInt32 Id;
#else
    typedef unsigned int Id;
#endif
    Id mId;
};

/////////////////////////////////////////////////////////////////////

} // end namespace snd

/////////////////////////////////////////////////////////////////////

#endif // sndengine_h
