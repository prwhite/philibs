/////////////////////////////////////////////////////////////////////
//
//	class: box
//
//	This hopefully trivial class will wrap a builtin type and do
//  basic initialization.  This will hopefully lead to less 
//  initializer code in constructors which is annoying and 
//  error prone.
//
//  Instances should act completely like their builtin counterparts
//  with the exception of having a valid, known initial state.
//
//  This thing has gotten a little crazy.  We're now using type-
//  dependent traits to concoct integer, pointer or even floating
//  point initialization values.  Scary.  TRICKY.
//
/////////////////////////////////////////////////////////////////////


#ifndef pnibox_h
#define pnibox_h

/////////////////////////////////////////////////////////////////////

// #include "pnipstddef.h"
// 
// #include "pnipstd.h"

#include <string>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {

/////////////////////////////////////////////////////////////////////

template< class Type, int Num >
class defInit
{
  public:
    operator Type () const { return ( Type ) Num; }
};

template< class Type, int Num, int Div >
class floatInit
{
  public:
    operator Type () const { return Num / ( Type ) Div; }
};


template< class Type, class Init = defInit< Type, 0 > >
class box
{
  public:
    box () : mVal ( Init () ) {}
    box ( Type const& val ) : mVal ( val ) {}
    ~box () {}  // Compiler's making me add this?!?!
    
    void reset () { mVal = Init (); }
    
    box& operator = ( box const& rhs ) { mVal = rhs.mVal; return *this; }
    box& operator = ( Type const& rhs ) { mVal = rhs; return *this; }
    bool operator == ( box const& rhs ) const { return mVal == rhs.mVal; }
    bool operator == ( Type const& rhs ) const { return mVal == rhs; }
    
    operator Type& () { return mVal; }
    operator Type const& () const { return mVal; }
    
    Type* operator & () { return &mVal; }
    Type const* operator & () const { return &mVal; }
    
      // For pointer types.
    Type operator -> ()  { return mVal; }
    Type const operator -> () const  { return mVal; }
    
      // Printf doesn't like the operators.
    Type const& get () const { return mVal; }
  
  private:
    Type mVal;
};


/////////////////////////////////////////////////////////////////////

	} // end of namespace pstd
} // end of namespace pni 

#endif // pnibox_h


