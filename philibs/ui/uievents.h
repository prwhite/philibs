/////////////////////////////////////////////////////////////////////
//
//    class: events
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuievents_h
#define uiuievents_h

/////////////////////////////////////////////////////////////////////

#include <string>

#include "uibase.h"

#include "scenetypes.h"

#include "sceneisectdd.h"

/////////////////////////////////////////////////////////////////////

namespace ui {
  class manager;
  
  typedef float TimeType;

/////////////////////////////////////////////////////////////////////

class event
{
  public:
    event () : mTimeStamp ( 0.0 ) {}
    virtual ~event () {}

    void setTimeStamp ( TimeType timeStamp ) { mTimeStamp = timeStamp; }
    TimeType getTimeStamp () const { return mTimeStamp; }

    typedef pni::pstd::autoRef< base > BaseRef;
    void setBase ( base* pBase ) { mBase = pBase; }
    base* getBase () const { return mBase.get (); }

    //typedef pni::pstd::autoRef< manager > ManagerRef;
    typedef manager* ManagerRef;
    void setManager ( manager* pManager ) { mManager = pManager; }
    manager* getManager () const { return mManager; }

  protected:
    TimeType mTimeStamp;
    BaseRef mBase;
    ManagerRef mManager;
};

/////////////////////////////////////////////////////////////////////

struct mousePointer
{
  enum Type {
      Uninit,
      Down,
      Up,
      Drag,
      Hold,
      SingleClick,
      DoubleClick,
      SingleClickOnly
  };

  void set ( float xpos, float ypos, Type type )
      {
        mXpos = xpos;
        mYpos = ypos;
        mType = type;
      }

  float mXpos;
  float mYpos;
  Type mType;
};

/////////////////////////////////////////////////////////////////////

struct mouseEvent :
  public event
{
  mouseEvent () :
    mActive ( 0 )
      {
        memsetPointers ();
      }

  void memsetPointers ()
      {
        mActive = 0;
        memset ( mMousePointers, 0, sizeof ( mMousePointers ) );
      }

  enum Vals {
    NumPointers = 10
  };

  mousePointer mMousePointers[ NumPointers ];
  unsigned char mActive;
 };

/////////////////////////////////////////////////////////////////////

struct accelEvent :
  public event
{
  accelEvent ( float xacc, float yacc, float zacc )
    : mXacc ( xacc ), mYacc ( yacc ), mZacc ( zacc )
      {}

  float mXacc;
  float mYacc;
  float mZacc;
};

/////////////////////////////////////////////////////////////////////

struct keyEvent :
  public event
{
  keyEvent ( std::string const& key, bool state ) :
    mKey ( key ), mState ( state )
      {}

  void set ( std::string const& key, bool state )
      {
        mKey = key;
        mState = state;
      }

  std::string mKey;
  bool mState;
};

/////////////////////////////////////////////////////////////////////
// Encapsulates the output of a pick seg on a scene sub-graph.

struct sceneEvent :
  public event
{
    scene::isectTest::Segs mSegs;
    scene::isectTest::Hits mHits;
};

/////////////////////////////////////////////////////////////////////

struct heartbeatEvent :
  public event
{
  
};

/////////////////////////////////////////////////////////////////////

// TODO:
// single tap event
// double tap events (both with multiple finger recognition)
// gestures:
//   swipe/flick
struct flickEvent :
  public event
{
  void set ( float xpos, float ypos, float xdir, float ydir )
    {
      mXpos = xpos;
      mYpos = ypos;
      mXdir = xdir;
      mYdir = ydir;
    }

  float mXpos;
  float mYpos;
  float mXdir;
  float mYdir;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuievents_h


