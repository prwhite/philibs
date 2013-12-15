/////////////////////////////////////////////////////////////////////
//
//    file: animbgmusicfader.cpp
//
/////////////////////////////////////////////////////////////////////

#include "animbgmusicfader.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

bgMusicFader::bgMusicFader ()
{
  // TODO
}

bgMusicFader::~bgMusicFader ()
{
  // TODO
}

//bgMusicFader::bgMusicFader ( bgMusicFader const& rhs )
//{
//  // TODO
//}
//
//bgMusicFader& bgMusicFader::operator= ( bgMusicFader const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool bgMusicFader::operator== ( bgMusicFader const& rhs ) const
//{
//  // TODO
//  return false;
//}

/////////////////////////////////////////////////////////////////////
//  overrides from base

//void bgMusicFader::internalUpdate ( manager* pManager, TimeType tval )
//{
//  using namespace pni::math;
//
//  float val = tval * mEndVal + ( 1.0f - tval ) * mStartVal;
//
//  if ( mStartVal == 0.0f && Trait::equal ( tval, 0.0f ) )
//  {
//    mEngine->startBgMusic ();
////printf ( "  bgMusicFader : startBgMusic\n" );
//  }
//
////printf ( "  bgMusicFader : val = %f\n", val );
//    mEngine->setBgMusicVolume ( tval );
//
//  if ( mEndVal == 0.0f && Trait::equal ( tval, 1.0f ) )
//  {
//    mEngine->unloadBgMusicTrack ();
////printf ( "  bgMusicFader : unloadBgMusicTrack\n" );
//  }
//}

/////////////////////////////////////////////////////////////////////

void bgMusicFadeIn::internalUpdate ( manager* pManager, TimeType tval )
{
  using namespace pni::math;

  if ( Trait::equal ( tval, 0.0f ) )
  {
//printf ( "  bgMusicFadeIn: startBgMusic\n" );
    mEngine->setBgMusicVolume ( 0.0f );
    mEngine->startBgMusic ();
  }

  float val = tval * 2.0f - 1.0f; // [-1 -> 1]

  if ( val > 0.0f )
  {
//printf ( "  bgMusicFadeIn : val = %f\n", val * mDstLevel );
    mEngine->setBgMusicVolume ( val * mDstLevel );
  }
}

/////////////////////////////////////////////////////////////////////

void bgMusicFadeOut::internalUpdate ( manager* pManager, TimeType tval )
{
  using namespace pni::math;

  float val = ( 1.0f - tval ) * 2.0f - 1.0f; // [1 -> -1]

  if ( val > 0.0f )
  {
//printf ( "  bgMusicFadeOut : val = %f\n", val );
    mEngine->setBgMusicVolume ( val * mSrcLevel );
  }

//printf ( " tval = %f\n", tval );

    // Note... fundamental problem with system...
    // tval doesn't always reach exactly 1.0 at end.
    // TODO: Fix this.
  if ( Trait::equal ( tval, 1.0f, 0.01f ) )
  {
//printf ( "  bgMusicFadeOut: unloadBgMusicTrack\n" );
    mEngine->unloadBgMusicTrack ();
  }

}


/////////////////////////////////////////////////////////////////////


} // end of namespace anim 


