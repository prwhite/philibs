/////////////////////////////////////////////////////////////////////
//
//  engine:: (
//
/////////////////////////////////////////////////////////////////////

#include "sndengine.h"

#include "SoundEngine.h"

#include <iostream>

/////////////////////////////////////////////////////////////////////

namespace snd
{

/////////////////////////////////////////////////////////////////////

engine::engine ()
{
  
}

/////////////////////////////////////////////////////////////////////

engine::engine ( float mixerOutputRate )
{
  ErrorType errorVal = SoundEngine_Initialize ( mixerOutputRate );
  
  if ( errorVal )
    std::cerr << "Error in " << __PRETTY_FUNCTION__ << ": " << errorVal << std::endl;
}

/////////////////////////////////////////////////////////////////////

engine::~engine ()
{
  ErrorType errorVal = SoundEngine_Teardown ();

  if ( errorVal )
    std::cerr << "Error in " << __PRETTY_FUNCTION__ << ": " << errorVal << std::endl;
}

/////////////////////////////////////////////////////////////////////


ErrorType engine::setMasterVolume ( float val )
{
  return SoundEngine_SetMasterVolume ( val );
}

/////////////////////////////////////////////////////////////////////

ErrorType engine::setListenerPos ( pni::math::vec3 const& pos )
{
  return SoundEngine_SetListenerPosition (
      pos[ 0 ], pos[ 1 ], pos[ 2 ] );
}

/////////////////////////////////////////////////////////////////////

ErrorType engine::setListenerGain ( float val )
{
  return SoundEngine_SetListenerGain ( val );
}

/////////////////////////////////////////////////////////////////////


ErrorType engine::loadBgMusicTrack ( std::string const& path, bool addToQueue, bool loadAtOnce )
{
  return SoundEngine_LoadBackgroundMusicTrack ( path.c_str (), addToQueue, loadAtOnce );
}

/////////////////////////////////////////////////////////////////////

ErrorType engine::unloadBgMusicTrack ()
{
  return SoundEngine_UnloadBackgroundMusicTrack ();
}

/////////////////////////////////////////////////////////////////////

ErrorType engine::startBgMusic ()
{
  return SoundEngine_StartBackgroundMusic ();
}

/////////////////////////////////////////////////////////////////////

ErrorType engine::stopBgMusic ( bool stopAtEnd )
{
  return SoundEngine_StopBackgroundMusic ( stopAtEnd );
}

/////////////////////////////////////////////////////////////////////

ErrorType engine::setBgMusicVolume ( float val )
{
  return SoundEngine_SetBackgroundMusicVolume ( val );
}

/////////////////////////////////////////////////////////////////////


ErrorType engine::setEffectsVolume ( float val )
{
  return SoundEngine_SetEffectsVolume ( val );
}

/////////////////////////////////////////////////////////////////////

ErrorType engine::setMaxDist ( float val )
{
  return SoundEngine_SetMaxDistance ( val );
}

/////////////////////////////////////////////////////////////////////

ErrorType engine::setRefDist ( float val )
{
  return SoundEngine_SetReferenceDistance ( val );
}

/////////////////////////////////////////////////////////////////////


ErrorType engine::doVibrate ()
{
  SoundEngine_Vibrate ();
  
  return 0;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


effect::effect ( std::string const& start, std::string const& loop, std::string const& end )
{
  char const* startPtr = ! start.empty () ? start.c_str () : 0;
  char const* endPtr = ! end.empty () ? end.c_str () : 0;

  ErrorType errorVal = SoundEngine_LoadLoopingEffect ( 
      loop.c_str (), startPtr, endPtr, &mId );
  setPitch ( 1.0f );
  setLevel ( 1.0f );
  setPos ( pni::math::vec3 () );
  
  if ( errorVal )
    std::cerr << "Error in " << __PRETTY_FUNCTION__ << ": " << errorVal << std::endl;
}

/////////////////////////////////////////////////////////////////////


effect::effect ( std::string const& fname )
{
  ErrorType errorVal = SoundEngine_LoadEffect ( fname.c_str (), &mId );
  setPitch ( 1.0f );
  setLevel ( 1.0f );
  setPos ( pni::math::vec3 () );
  
  if ( errorVal )
    std::cerr << "Error in " << __PRETTY_FUNCTION__ << ": " << errorVal << std::endl;
}

/////////////////////////////////////////////////////////////////////


effect::~effect ()
{
  ErrorType errorVal = SoundEngine_UnloadEffect ( mId );
  
  if ( errorVal )
    std::cerr << "Error in " << __PRETTY_FUNCTION__ << ": " << errorVal << std::endl;
}

/////////////////////////////////////////////////////////////////////


ErrorType effect::start ()
{
  return SoundEngine_StartEffect ( mId );
}

/////////////////////////////////////////////////////////////////////

ErrorType effect::stop ( bool doEnd )
{
  return SoundEngine_StopEffect ( mId, doEnd );
}

/////////////////////////////////////////////////////////////////////


ErrorType effect::setPitch ( float val )
{
  return SoundEngine_SetEffectPitch ( mId, val );
}

/////////////////////////////////////////////////////////////////////

ErrorType effect::setLevel ( float val )
{
  return SoundEngine_SetEffectLevel ( mId, val );
}

/////////////////////////////////////////////////////////////////////

ErrorType effect::setPos ( pni::math::vec3 const& pos )
{
  return SoundEngine_SetEffectPosition ( mId,
        pos[ 0 ], pos[ 1 ], pos[ 2 ] );
}

/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

} // end namespace snd
