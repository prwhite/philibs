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
#include "pnimathstream.h"
#include "pnirand.h"
#include "pnimatstack.h"
#include "pniplane.h"
#include "pnisphere.h"
#include "pnirand.h"
#include "pnifrustum.h"
#include <iostream>

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
}


// Quick test of ProjectOnto.
void projectOntoTest ()
{
	vec2 diag2 ( 2, 2 );
	vec2 xAxis2 ( 1, 0 );

	diag2.projectOnto ( xAxis2 );

	std::cout << "( 2, 2 ) projectOnto ( 1, 0 ) = ( " << diag2 << " )\n";


	vec3 diag3 ( 2, 2, 2 );
	vec3 xAxis3 ( 1, 0, 0 );

	diag3.projectOnto ( xAxis3 );

	std::cout << "( 2, 2, 2 ) projectOnto ( 1, 0, 0 ) = ( " << diag3 << " )\n";


	vec4 diag4 ( 2, 2, 2, 2 );
	vec4 xAxis4 ( 1, 0, 0, 0 );

	diag4.projectOnto ( xAxis4 );

	std::cout << "( 2, 2, 2, 2 ) projectOnto ( 1, 0, 0, 0 ) = ( " << diag4 << " )\n";
}



template< class testType >
void frustumTest ( const testType& testObj )
{
	std::cout << "frustumTest(): testObj=" << &testObj << std::endl;
}

void scaleTest ()
{
	std::cout << "begin scale test" << std::endl;

	vec3 v3 ( getTheRand () );
	vec3 eul ( getTheRand () );
	matrix4 m4;

	for ( int num = 0; num < 1000; num++ )
	{
		m4.setIdentity ();

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

		if ( ! v3.equal ( vec3 ( 1, 1, 1 ) ) )
			std::cout << "mat scale out (should be 1,1,1): " << v3 << std::endl;
	}

	std::cout << "end scale test" << std::endl;
}

void
matStackTest ()
{
	matrix4 tmp;
	matStack< matrix4 > mstack ( matStack< matrix4 >::NoInit );
	mstack->setIdentity ();

	std::wcout << L"init: " << tmp << std::endl;

	mstack.push ();
	
	std::wcout << L"push: " << *mstack << std::endl;

	tmp.setEuler ( vec3 ( 30.0f, 50.0f, 12.0f ) );

	*mstack *= tmp;

	std::wcout << L"mult: " << *mstack << std::endl;

	mstack.push ();

	*mstack *= tmp;

	std::wcout << L"push & mult: " << *mstack << std::endl;

	mstack.pop ();

	std::wcout << L"pop: " << *mstack << std::endl;

	mstack.pop ();

	std::wcout << L"pop: " << *mstack << std::endl;
}

void
lookatTest ()
{
	using namespace std;
	using namespace pni;

	wcout << L"lookatTest begin" << endl;

	vec3 from ( 0.0f, 1.0f, 0.0f );
	vec3 to ( 1.0f, 1.0f, 0.0f );
	vec3 up ( 0.0f, 0.0f, 1.0f );

	matrix4 mat;
	mat.setLookat ( from, to, up );

	wcout << L"mat: " << mat << endl;
	
	vec3 eul;
	mat.getCoord ( from, eul );

	wcout << L"coord: " << from << L" " << eul << endl;

	wcout << L"lookatTest end" << endl;
}

void
planeContainsSphereTest ()
{
	using namespace pni;
	using namespace pni::math;
	using namespace std;

	wcout << L"planeContainsSphereTest begin" << endl;

	plane pln  ( 1.0f, 0.0f, 0.0f, -1.0f );
	sphere sp1 ( 0.0f, 0.0f, 0.0f, 0.5f );
	sphere sp2 ( 0.0f, 0.0f, 0.0f, 1.1f );
	sphere sp3 ( -2.0f, 0.0f, 0.0f, 0.5f );

	if ( pln.contains ( sp1 ) != ( containsResult::SomeIn | containsResult::AllIn ) ) throw;

	if ( pln.contains ( sp2 ) != containsResult::SomeIn ) throw;	// testing fp precision

	if ( pln.contains ( sp3 ) != containsResult::NoneIn ) throw;

	matrix4 mat4;
	mat4.setRot ( 90.0f, 0.0f, 0.0f, 1.0f );

	pln.xformOrtho ( pln, mat4 );

	if ( pln.contains ( sp1 ) != ( containsResult::SomeIn | containsResult::AllIn ) ) throw;

	if ( pln.contains ( sp2 ) != containsResult::SomeIn ) throw;	// testing fp precision

	if ( pln.contains ( sp3 ) != ( containsResult::SomeIn | containsResult::AllIn ) ) throw;

	wcout << L"planeContainsSphereTest begin" << endl;
}


void
planeContainsSphereTest2 ()
{
	using namespace pni;
	using namespace pni::math;
	using namespace std;

	wcout << L"planeContainsSphereTest2 begin" << endl;

	for ( int num = 0; num < 256; ++num )
	{
		plane pln  ( 1.0f, 0.0f, 0.0f, 2.0f );
		sphere sp1 ( 2.0f, 0.0f, 0.0f, 0.5f );
		sphere sp2 ( 4.0f, 4.0f, 4.0f, 1.0f );
		sphere sp3 ( -2.0f, 0.0f, 0.0f, 0.5f );

		if ( pln.contains ( sp1 ) != ( containsResult::SomeIn ) ) throw;

		if ( pln.contains ( sp2 ) != ( containsResult::SomeIn | containsResult::AllIn ) ) throw;

		if ( pln.contains ( sp3 ) != containsResult::NoneIn ) throw;

		matrix4 mat4;
		mat4.setRot ( getTheRand (), getTheRand () );

		pln.xformOrtho ( pln, mat4 );
		sp1.xformOrtho ( sp1, mat4 );
		sp2.xformOrtho ( sp2, mat4 );
		sp3.xformOrtho ( sp3, mat4 );

		if ( pln.contains ( sp1 ) != ( containsResult::SomeIn ) ) throw;

		if ( pln.contains ( sp2 ) != ( containsResult::SomeIn | containsResult::AllIn ) ) throw;

		if ( pln.contains ( sp3 ) != containsResult::NoneIn ) throw;
	}

	wcout << L"planeContainsSphereTest2 begin" << endl;
}


void
frustumContainsSphereTest ()
{
	using namespace pni;
	using namespace pni::math;
	using namespace std;

	wcout << L"frustumContainsSphereTest begin" << endl;

	for ( int num = 0; num < 256; ++num )
	{
		frustum frust ( 0.1f, 100.0f, -0.1f, 0.1f, -0.1f, 0.1f );
		matrix4 mat4;
		mat4.setTrans ( 0.0f, 2.0f, 0.0f );
		frust.xformOrtho ( frust, mat4 );

		sphere sp1 ( 0.0f, 2.0f, 0.0f, 0.5f );
		sphere sp2 ( 0.0f, 4.0f, 0.0f, 1.0f );
		sphere sp3 ( 0.0f, -2.0f, 0.0f, 0.5f );

		if ( frust.contains ( sp1 ) != ( containsResult::SomeIn ) ) throw;

		if ( frust.contains ( sp2 ) != ( containsResult::SomeIn | containsResult::AllIn ) ) throw;

		if ( frust.contains ( sp3 ) != containsResult::NoneIn ) throw;

		mat4.setRot ( getTheRand (), getTheRand () );

		frust.xformOrtho ( frust, mat4 );
		sp1.xformOrtho ( sp1, mat4 );
		sp2.xformOrtho ( sp2, mat4 );
		sp3.xformOrtho ( sp3, mat4 );

		if ( frust.contains ( sp1 ) != ( containsResult::SomeIn ) ) throw;

		if ( frust.contains ( sp2 ) != ( containsResult::SomeIn | containsResult::AllIn ) ) throw;

		if ( frust.contains ( sp3 ) != containsResult::NoneIn ) throw;
	}

	wcout << L"frustumContainsSphereTest begin" << endl;
}

void
sphereExtendByVecTest ()
{
	using namespace pni;
	using namespace pni::math;
	using namespace std;

	wcout << L"sphereExtendByVecTest begin" << endl;

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

	wcout << L"sphereExtendByVecTest end" << endl;

}

void
sphereExtendBySphereTest ()
{
	using namespace pni;
	using namespace pni::math;
	using namespace std;

	wcout << L"sphereExtendBySphereTest begin" << endl;

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

	wcout << L"sphereExtendBySphereTest end" << endl;
}

void
sphereExtendByVecTest2 ()
{
	using namespace pni;
	using namespace pni::math;
	using namespace std;

	wcout << L"sphereExtendByVecTest2 begin" << endl;

	sphere sp;

	for ( int num = 0; num < 22256; ++num )
	{
		vec3 pt ( getTheRand () ( vec3 ( -1.0f, -1.0f, -1.0f ) * 5.0f, vec3 ( 1.0f, 1.0f, 1.0f ) * 5.0f ) );
		sp.extendBy ( pt );
	}

	wcout << L"sphereExtendByVecTest2 end" << endl;
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

	return 0;
}

