/////////////////////////////////////////////////////////////////////
//
//	rand.C
//
/////////////////////////////////////////////////////////////////////

#include "pnirand.h"
#include <ctime>
//#include <cstdlib>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

rand&
getTheRand ()
{
	static rand theRand;

	return theRand;
}

/////////////////////////////////////////////////////////////////////

rand::
rand () :
  mDbg ( false ),
  mDbgCounter ( 0 )
{
#ifdef OLDWAY
	seed = time ( 0 );
	srand ( seed );
#else
  mRand.seed ( ( MTRand::uint32 ) time ( 0 ) );
#endif
}

rand::
rand ( int seedIn ) :
#ifdef OLDWAY
	seed ( seedIn )
#else
  mRand ( ( MTRand::uint32 ) seedIn ),
  mDbg ( false ),
  mDbgCounter ( 0 )
#endif
{
#ifdef OLDWAY
	srand ( seed );
#else
  //mRand.seed ( seedIn );
#endif
}

/////////////////////////////////////////////////////////////////////

void
rand::
reseed ()
{
#ifdef OLDWAY
	srand ( seed );
#else
  mRand.seed ();  // TODO: Semantic is not the same as OLDWAY.
#endif
}

void
rand::
reseed ( int seedIn )
{
#ifdef OLDWAY
	seed = seedIn;
	srand ( seed );
#else
  mRand.seed ( seedIn );
#endif
}

/////////////////////////////////////////////////////////////////////
// egcs required the scoping of ValueType below... bummer PNI

rand::
operator rand::ValueType ()
{
#ifdef OLDWAY
	return ( ValueType ) ::rand () / RAND_MAX;	
#else
  return ( ValueType ) operator int () / MaxRandInt;
#endif
}

rand::
operator int ()
{
#ifdef OLDWAY
	return ::rand ();
#else
  int retVal = mRand.randInt () & MaxRandInt;
#ifndef _NDEBUG
  if ( mDbg )
  {
    printf ( " rand: %zd : %d\n", mDbgCounter, retVal );
    ++mDbgCounter;
  }
#endif // _NDEBUG
  return retVal;
#endif // OLDWAY
}

rand::
operator vec3 ()
{
	return vec3 ( *this, *this, *this );
}

rand::ValueType
rand::
operator () ( ValueType min, ValueType max )
{
	return ( operator ValueType () ) * ( max - min ) + min;	
}

int
rand::
operator () ( int min, int max )
{
	if ( min != max )
		return ( ( operator int () ) % ( max - min ) ) + min;
	else
		return min;
}

size_t 
rand::
sizeT ( size_t min, size_t max )
{
  if ( min != max )
    return ( ( size_t ) ( operator int () ) ) % ( max - min ) + min;
  else
    return min;
}



vec3
rand::
operator () ( const vec3& min, const vec3& max )
{
	return vec3 ( ( *this ) ( min[ 0 ], max[ 0 ] ), 
					  ( *this ) ( min[ 1 ], max[ 1 ] ), 
					  ( *this ) ( min[ 2 ], max[ 2 ] ) );
}

/////////////////////////////////////////////////////////////////////

bool
rand::
operator () ( ValueType percent )
{
	if ( operator ValueType () <= percent )
		return true;
	else
		return false;
}

bool
rand::
operator () ( int percent )
{
	if ( operator () ( 0, 101 ) <= percent )
		return true;
	else
		return false;
}

bool
rand::
operator () ()
{
	if ( operator () ( 0, 2 ) )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni


