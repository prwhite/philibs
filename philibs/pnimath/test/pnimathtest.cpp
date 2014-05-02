////////////////////////////////////////////////////////////////////
//
// file: pnimathtest.cpp
//
// Defines the entry point for the console test application.
//
////////////////////////////////////////////////////////////////////

#include "pniquat.h"
#include "pnivec4.h"
#include "pnivec3.h"
#include "pnivec2.h"
#include "pnimatrix4.h"
#include "pnirand.h"
#include "pnimatstack.h"
#include "pniplane.h"
#include "pnisphere.h"
#include "pnirand.h"
#include "pnifrustum.h"

#include "QUnit.hpp"
#include <iostream>
#include "pnimathstream.h"

QUnit::UnitTest qunit(std::cerr, QUnit::normal);

using namespace pni;
using namespace pni::math;

// Just a quick Euler angle / quaternion conversion test.
void eularQuatTest ()
{
  quat quaternion;

  quaternion.setEuler ( 60, 30, 0 );

  std::cout << "Setting quat to euler ( 60, 30, 0 )\n";

  float heading, pitch, roll;

  quaternion.getEuler ( heading, pitch, roll );

  std::cout << "Getting euler from quat ( " << heading << ", " << pitch << ", " << roll << " )\n";

  QUNIT_IS_TRUE(Trait::equal(60.0f,heading));
  QUNIT_IS_TRUE(Trait::equal(30.0f,pitch));
  QUNIT_IS_TRUE(Trait::equal( 0.0f,roll,0.000001f)); // bigger epsilon for test
}

// Quick test of ProjectOnto.
void projectOntoTest ()
{
  vec2 diag2 ( 2, 2 );
  vec2 xAxis2 ( 1, 0 );

  diag2.projectOnto ( xAxis2 );

  std::cout << "( 2, 2 ) projectOnto ( 1, 0 ) = ( " << diag2 << " )\n";
  QUNIT_IS_TRUE(diag2.equal(vec2(2.0f,0.0f)));

  vec3 diag3 ( 2, 2, 2 );
  vec3 xAxis3 ( 1, 0, 0 );

  diag3.projectOnto ( xAxis3 );

  std::cout << "( 2, 2, 2 ) projectOnto ( 1, 0, 0 ) = ( " << diag3 << " )\n";
  QUNIT_IS_TRUE(diag3.equal(vec3(2.0f,0.0f,0.0f)));

  vec4 diag4 ( 2, 2, 2, 2 );
  vec4 xAxis4 ( 1, 0, 0, 0 );

  diag4.projectOnto ( xAxis4 );

  std::cout << "( 2, 2, 2, 2 ) projectOnto ( 1, 0, 0, 0 ) = ( " << diag4 << " )\n";
  QUNIT_IS_TRUE(diag4.equal(vec4(2.0f,0.0f,0.0f,0.0f)));
}



template< class testType >
void frustumTest ( const testType& testObj )
{
  std::cout << "frustumTest(): testObj=" << &testObj << std::endl;
}

void scaleTest ()
{
  std::cout << "begin scale test" << std::endl;
  std::cout << "FIXME: THIS TEST IS CURRENTLY BOGUS!!!" << std::endl;

  matrix4 m4;

  for ( int num = 0; num < 1000; num++ )
  {
    m4.setIdentity ();
    vec3 v3 ( getTheRand () );
    vec3 eul ( getTheRand () );

    // scale and rotate a few times
    m4.postScale ( v3 * 2.0f );
    m4.postEuler ( eul * 2.0f );
    m4.postScale ( v3 );
    m4.postEuler ( eul );

    // get scale out
    m4.getScale ( v3 );

    // invert the scale
    v3.invert ();

    // cancel the scale out
    m4.postScale ( v3 );

    // get scale back out and make sure it was properly cancelled out
    m4.getScale ( v3 );

      // Temp removed because the test isn't set up correctly currently
    // QUNIT_IS_TRUE ( v3.equal ( vec3 ( 1, 1, 1 ) ) );
  }

  std::cout << "end scale test" << std::endl;
}

void
matStackTest ()
{
  std::cout << "begin matStackTest test" << std::endl;

  matrix4 tmp;
  matStack< matrix4 > mstack ( matStack< matrix4 >::NoInit );

  QUNIT_IS_EQUAL(mstack.getCurDepth(),0);

  mstack->setIdentity ();

  std::cout << "init:\n" << tmp << std::endl;

  mstack.push ();
  
  std::cout << "push:\n" << *mstack << std::endl;

  tmp.setEuler ( vec3 ( 30.0f, 50.0f, 12.0f ) );

  *mstack *= tmp;

  std::cout << "mult:\n" << *mstack << std::endl;

  mstack.push ();

  *mstack *= tmp;

  std::cout << "push & mult:\n" << *mstack << std::endl;

  mstack.pop ();

  std::cout << "pop:\n" << *mstack << std::endl;

  mstack.pop ();

  std::cout << "pop:\n" << *mstack << std::endl;

  QUNIT_IS_EQUAL(mstack.getCurDepth(),0);

  std::cout << "end matStackTest test" << std::endl;
}

void
lookatTest ()
{
  using namespace std;
  using namespace pni;

  cout << "lookatTest begin" << endl;

  {
    vec3 from ( 0.0f, 0.0f, 0.0f );
    vec3 to ( 0.0f, 0.0f, -1.0f );
    vec3 up ( 0.0f, 1.0f, 0.0f );

    matrix4 mat;
    mat.setLookat ( from, to, up );

    cout << "mat:\n" << mat << endl;
    
    vec3 eul;
    mat.getCoord ( from, eul );

    cout << "coord:\n" << from << " " << eul << endl;
    QUNIT_IS_TRUE(from.equal(vec3(0.0f,0.0f,0.0f)));
    QUNIT_IS_TRUE(eul.equal(vec3(0.0f,0.0f,0.0f)));
  }

  {
    vec3 from ( 1.0f, 1.0f, 1.0f );
    vec3 to ( 2.0f, 1.0f, 1.0f );
    vec3 up ( 0.0f, 1.0f, 0.0f );

    matrix4 mat;
    mat.setLookat ( from, to, up );

    cout << "mat:\n" << mat << endl;
    
    vec3 eul;
    mat.getCoord ( from, eul );

    cout << "coord:\n" << from << " " << eul << endl;
    QUNIT_IS_TRUE(from.equal(vec3(1.0f,1.0f,1.0f)));
    QUNIT_IS_TRUE(eul.equal(vec3(-90.0f,0.0f,0.0f)));
  }
  cout << "lookatTest end" << endl;
}

void
planeContainsSphereTest ()
{
  using namespace pni;
  using namespace pni::math;
  using namespace std;

  cout << "planeContainsSphereTest begin" << endl;

  plane pln  ( 1.0f, 0.0f, 0.0f, -1.0f );
  sphere sp1 ( 0.0f, 0.0f, 0.0f, 0.5f );
  sphere sp2 ( 0.0f, 0.0f, 0.0f, 1.1f );
  sphere sp3 ( -2.0f, 0.0f, 0.0f, 0.5f );

  QUNIT_IS_FALSE ( pln.contains ( sp1 ) != ( containsResult::SomeIn | containsResult::AllIn ) );

  QUNIT_IS_FALSE ( pln.contains ( sp2 ) != containsResult::SomeIn );  // testing fp precision

  QUNIT_IS_FALSE ( pln.contains ( sp3 ) != containsResult::NoneIn );

  matrix4 mat4;
  mat4.setRot ( 90.0f, 0.0f, 0.0f, 1.0f );

  pln.xformOrtho ( pln, mat4 );

  QUNIT_IS_FALSE ( pln.contains ( sp1 ) != ( containsResult::SomeIn | containsResult::AllIn ) );

  QUNIT_IS_FALSE ( pln.contains ( sp2 ) != containsResult::SomeIn );  // testing fp precision

  QUNIT_IS_FALSE ( pln.contains ( sp3 ) != ( containsResult::SomeIn | containsResult::AllIn ) );

  cout << "planeContainsSphereTest begin" << endl;
}


void
planeContainsSphereTest2 ()
{
  using namespace pni;
  using namespace pni::math;
  using namespace std;

  cout << "planeContainsSphereTest2 begin" << endl;

  for ( int num = 0; num < 256; ++num )
  {
    plane pln  ( 1.0f, 0.0f, 0.0f, 2.0f );
    sphere sp1 ( 2.0f, 0.0f, 0.0f, 0.5f );
    sphere sp2 ( 4.0f, 4.0f, 4.0f, 1.0f );
    sphere sp3 ( -2.0f, 0.0f, 0.0f, 0.5f );

    QUNIT_IS_FALSE ( pln.contains ( sp1 ) != ( containsResult::SomeIn ) );

    QUNIT_IS_FALSE ( pln.contains ( sp2 ) != ( containsResult::SomeIn | containsResult::AllIn ) );

    QUNIT_IS_FALSE ( pln.contains ( sp3 ) != containsResult::NoneIn );

    matrix4 mat4;
    mat4.setRot ( getTheRand (), getTheRand () );

    pln.xformOrtho ( pln, mat4 );
    sp1.xformOrtho ( sp1, mat4 );
    sp2.xformOrtho ( sp2, mat4 );
    sp3.xformOrtho ( sp3, mat4 );

    QUNIT_IS_FALSE ( pln.contains ( sp1 ) != ( containsResult::SomeIn ) );

    QUNIT_IS_FALSE ( pln.contains ( sp2 ) != ( containsResult::SomeIn | containsResult::AllIn ) );

    QUNIT_IS_FALSE ( pln.contains ( sp3 ) != containsResult::NoneIn );
  }

  cout << "planeContainsSphereTest2 begin" << endl;
}


void
frustumContainsSphereTest ()
{
  using namespace pni;
  using namespace pni::math;
  using namespace std;

  cout << "frustumContainsSphereTest begin" << endl;

  frustum frust ( 0.1f, 100.0f, -0.1f, 0.1f, -0.1f, 0.1f );
  matrix4 mat4;
  mat4.setTrans ( 0.0f, 2.0f, 0.0f );
  frust.xformOrtho ( frust, mat4 );

  sphere sp1 ( 0.0f, 2.0f, 0.0f, 0.5f );
  sphere sp2 ( 0.0f, 4.0f, 0.0f, 1.0f );
  sphere sp3 ( 0.0f, -2.0f, 0.0f, 0.5f );

  QUNIT_IS_FALSE ( frust.contains ( sp1 ) != ( containsResult::SomeIn ) );

  QUNIT_IS_FALSE ( frust.contains ( sp2 ) != ( containsResult::SomeIn | containsResult::AllIn ) );

  QUNIT_IS_FALSE ( frust.contains ( sp3 ) != containsResult::NoneIn );

  mat4.setRot ( getTheRand (), getTheRand () );

  frust.xformOrtho ( frust, mat4 );
  sp1.xformOrtho ( sp1, mat4 );
  sp2.xformOrtho ( sp2, mat4 );
  sp3.xformOrtho ( sp3, mat4 );

  QUNIT_IS_FALSE ( frust.contains ( sp1 ) != ( containsResult::SomeIn ) );

  QUNIT_IS_FALSE ( frust.contains ( sp2 ) != ( containsResult::SomeIn | containsResult::AllIn ) );

  QUNIT_IS_FALSE ( frust.contains ( sp3 ) != containsResult::NoneIn );

  cout << "frustumContainsSphereTest begin" << endl;
}

void
sphereExtendByVecTest ()
{
  using namespace pni;
  using namespace pni::math;
  using namespace std;

  cout << "sphereExtendByVecTest begin" << endl;

  vec3 points[ 6 ] = { 
      vec3 ( 1.0f, 0.0f, 0.0f ),
      vec3 ( -1.0f, 0.0f, 0.0f ),
      vec3 ( 0.0f, 1.0f, 0.0f ),
      vec3 ( 0.0f, -1.0f, 0.0f ),
      vec3 ( 0.0f, 0.0f, 1.0f ),
      vec3 ( 0.0f, 0.0f, -1.0f )
  };

  sphere sp;

  for ( int num = 0; num < 6; ++num )
    sp.extendBy ( points[ num ] );

  QUNIT_IS_EQUAL(sp.getRadius(),1.0f);

  cout << "sphereExtendByVecTest end" << endl;
}

void
sphereExtendBySphereTest ()
{
  using namespace pni;
  using namespace pni::math;
  using namespace std;

  cout << "sphereExtendBySphereTest begin" << endl;

  vec3 points[ 6 ] = { 
      vec3 ( 1.0f, 0.0f, 0.0f ),
      vec3 ( -1.0f, 0.0f, 0.0f ),
      vec3 ( 0.0f, 1.0f, 0.0f ),
      vec3 ( 0.0f, -1.0f, 0.0f ),
      vec3 ( 0.0f, 0.0f, 1.0f ),
      vec3 ( 0.0f, 0.0f, -1.0f )
  };

  sphere sp;

  for ( int num = 0; num < 6; ++num )
  {
    sphere sp2 ( points[ num ], 1.0f );
    sp.extendBy ( sp2 );
  }

  QUNIT_IS_EQUAL(sp.getRadius(),2.0f);

  cout << "sphereExtendBySphereTest end" << endl;
}

void
sphereExtendByVecTest2 ()
{
  using namespace pni;
  using namespace pni::math;
  using namespace std;

  cout << "sphereExtendByVecTest2 begin" << endl;

  sphere sp;

  for ( int num = 0; num < 22256; ++num )
  {
    vec3 pt ( getTheRand () ( vec3 ( -1.0f, -1.0f, -1.0f ) * 5.0f, vec3 ( 1.0f, 1.0f, 1.0f ) * 5.0f ) );
    sp.extendBy ( pt );
  }

  QUNIT_IS_TRUE(sp.getRadius() <= 10.0f);

  cout << "sphereExtendByVecTest2 end" << endl;
}

void
testMirror()
{
  using namespace std;
  cout << "testMirror begin" << endl;

  pni::math::plane pl ( pni::math::vec3 ( 0.0f, 0.0f, 1.0f ), 0.0f );
  pni::math::vec3 srcdst ( 1.0f, 1.0f, 1.0f );
  pl.mirror ( srcdst, srcdst );
  QUNIT_IS_EQUAL(srcdst,pni::math::vec3(1.0f,1.0f,-1.0f));

  pl.setNormPt( { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );
  pl.mirror ( srcdst, srcdst );
  QUNIT_IS_EQUAL(srcdst,pni::math::vec3(1.0f,-1.0f,-1.0f));

  pl.setNormPt( { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );
  pl.mirror ( srcdst, srcdst );
  QUNIT_IS_EQUAL(srcdst,pni::math::vec3(-1.0f,-1.0f,-1.0f));

  pl.setNormPt( { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 4.0f } );
  pni::math::matrix4 mat;
  mat.setMirror( pl );
  pni::math::vec3 pt0 ( 1.0f, 1.0f, 8.0f );
  pt0.xformPt(pt0, mat);

  QUNIT_IS_EQUAL(pt0,pni::math::vec3(1.0f,1.0f,0.0f));

  cout << "testMirror end" << endl;
}


int
main ( int argc, char* argv[] )
{
  eularQuatTest ();
  projectOntoTest ();
  scaleTest ();
  matStackTest ();
  lookatTest ();
  frustumContainsSphereTest ();

  sphereExtendByVecTest ();
  sphereExtendBySphereTest ();
  sphereExtendByVecTest2 ();

  testMirror();

  return 0;
}

