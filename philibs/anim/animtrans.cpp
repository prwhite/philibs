/////////////////////////////////////////////////////////////////////
//
//    file: animtrans.cpp
//
/////////////////////////////////////////////////////////////////////

#include "animtrans.h"

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

//trans::trans ()
//{
//  // TODO
//}
//
//trans::~trans ()
//{
//  // TODO
//}
//
//trans::trans ( trans const& rhs )
//{
//  // TODO
//}
//
//trans& trans::operator= ( trans const& rhs )
//{
//  // TODO
//  return *this;
//}
//
//bool trans::operator== ( trans const& rhs ) const
//{
//  // TODO
//  return false;
//}

/////////////////////////////////////////////////////////////////////
//  overrides from base
void trans::initId ()
{
  setId ( mHelper.get () );
}

/////////////////////////////////////////////////////////////////////

void trans::initStartFromCur ()
{
  if ( mKeys.empty () )
    mKeys.resize ( 1 );
  getMatrix ().getTrans ( mKeys[ 0 ] );
}

/////////////////////////////////////////////////////////////////////
// overrides from base

void trans::internalUpdate ( manager* pManager, TimeType tval )
{
    // tval [0.0,1.0]
    
  if ( mKeys.size () < 2 )
    return;

  if ( getExponent () > 1.0f )
    tval = useExponent ( pManager, tval );

  // Handle interp between appropriate keys.
  
    // mult [0.0,mKeys.size - 1.0]
    
  TimeType mult = mKeys.size () - 1.0f;

  // TRICKY: There is a possible fp error issue here.  mult * tval
  // could be exactly mKeys.size () making ubIndex index past the
  // end of the keys array.

  TimeType lb = mult * tval;
  if ( lb < 0.0f )
    lb = 0.0f;
  size_t lbIndex = ( size_t ) lb;
  
  if ( lbIndex >= mKeys.size () - 1 )
    lbIndex = mKeys.size () - 2;
  
  size_t ubIndex = lbIndex + 1;
  
//   void combine ( 
//     ValueType t1, const ThisType& v1, ValueType t2, const ThisType& v2 );

  mHelper->transOp ().combine ( 1.0f - tval, mKeys[ lbIndex ], tval, mKeys[ ubIndex ] );

// std::cout << "  t = " << tval << " pos = " << mHelper->transOp () << std::endl;
}

/////////////////////////////////////////////////////////////////////

void trans::collectRefs ( pni::pstd::refCount::Refs& refs ) const
{
  base::collectRefs ( refs );
  refs.push_back ( mHelper.get () );
}

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 


