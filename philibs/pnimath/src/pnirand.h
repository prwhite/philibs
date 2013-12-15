/////////////////////////////////////////////////////////////////////
//
//	class: rand
//
//	This class is an experiment in exploiting some of the syntactic
//	sugar of C++.  Of course, when the type is ambiguous, the
//	syntax gets ugly ( casts ):
//
//	rand arand;
//	ValueType  aa = arand;				// ok
//	int    bb = arand;				// ok
//	double cc = arand;				// error, type is ambiguous
//	double dd = ( ValueType ) arand;	// ok, specific type is present
//	
//	One of the solutions is to have methods for every type.  This
//	is acceptable until you get methods with one or more arguments.
//	To support all combinations of types in these situations an
//	explosion of methods would occur which just sucks.
//	
/////////////////////////////////////////////////////////////////////


#ifndef pnirand_h
#define pnirand_h

/////////////////////////////////////////////////////////////////////

#include "pnimath.h"
#include "pnivec3.h"

  // http://www-personal.umich.edu/~wagnerr/MersenneTwister.html
#include "MersenneTwister.h"

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace math {

/////////////////////////////////////////////////////////////////////

class PNIMATHAPI rand { 
	public:
		typedef Trait TraitType;
		typedef TraitType::ValueType ValueType;
		typedef ValueType& RefType;
		typedef const ValueType& ConstRefType;
		typedef rand ThisType;
	
	  enum Vals { MaxRandInt = 0x7fffffffUL };
	
		rand ();
		rand ( int seedIn );
		void reseed ();
		void reseed ( int seedIn );
		
		// use rand object in place of int, ValueType, vec
		operator ValueType ();			// users may sometimes need to cast
		operator int ();				//  therand to get the right type to
		operator vec3 ();				//  be called for these conversions
		
		// use function call operator for getting rands in a range
		ValueType operator () ( ValueType min, ValueType max );		// [min,max]
		int operator () ( int min, int max );						// [min,max)
		vec3 operator () ( const vec3& min, const vec3& max );

		size_t sizeT ( size_t min, size_t max );						// [min,max)
		
		// use these operators for a trigger... it will return true
		// 'percent' of the time, otherwise false
		bool operator () ( ValueType percent0to1 );
		bool operator () ( int percent0to100 );
		
		// this is a bool operator, equivalent to operator () ( 50 )
		bool operator () ();
		
		  // TEMP: Debug random number generation.
		void setDbgEnable ( bool val ) { mDbg = val; }
		bool getDbgEnable () const { return mDbg; }
		
	protected:
		//int seed;
		
		bool mDbg;
		size_t mDbgCounter;
		
		MTRand mRand;
		
	private:
	
};

/////////////////////////////////////////////////////////////////////

PNIMATHAPI rand& getTheRand ();

/////////////////////////////////////////////////////////////////////

	} // end namespace math
} // end namespace pni

/////////////////////////////////////////////////////////////////////

#endif // pnirand_h
