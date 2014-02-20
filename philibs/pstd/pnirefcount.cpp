/////////////////////////////////////////////////////////////////////
//
//	file: pnirefcount.cpp
//
/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"

// #include "pniatomicplatform.h"

#include <cassert>

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#include "pnirefcountdbg.h"

namespace 
{
  typedef pni::pstd::dbgRefCount::Ref Ref;
  typedef pni::pstd::dbgRefCount dbgRefCount;
  typedef pni::pstd::dbgRefCount::Refs Refs;
}

//#define PNIREFCOUNTDBG
#ifdef PNIREFCOUNTDBG

#include <set>
#include <map>
#include <map>
#include <iostream>
#include <string>

////////

namespace
{
  typedef std::set< Ref* > RefSet;
  typedef std::map< Ref*, Refs > FwdRefMap;
  typedef std::multimap< Ref*, Ref* > RevRefMap;

  RefSet sRefs;
  FwdRefMap sFwdRefs;
  RevRefMap sRevRefs;
  RefSet sOrphans;
  
  Ref* sTrackRef = 0;
  
  std::ostream* sStr = &std::cout;

  void onTrackRef ( Ref* pRef )
      {
        std::string name = typeid ( *pRef ).name ();
//        int numRefs = pRef->getNumRefs ();
      }

  void dbgNewRef ( Ref* pRef )
      {
        if ( sTrackRef == pRef )
          onTrackRef ( pRef );
      
        sRefs.insert ( pRef );
      }
      
  void dbgDelRef ( Ref* pRef )
      {
        if ( sTrackRef == pRef )
          onTrackRef ( pRef );

        sRefs.erase ( pRef );
      }

  void dbgRef ( Ref* pRef )
      {
        if ( pRef == sTrackRef )
          onTrackRef ( pRef );
      }
      
  void dbgUnref ( Ref* pRef )
      {
        if ( pRef == sTrackRef )
          onTrackRef ( pRef );
      }
  
//  void printFwdRefs ()
//      {
//      
//      }
  
  void printRevRefs ()
      {
        RevRefMap::iterator start = sRevRefs.begin ();
        RevRefMap::iterator end = sRevRefs.upper_bound ( start->first );
        
        while ( start != end )
        {
          *sStr << " rev refs for ";
          pni::pstd::dbgRefCount::doDbg ( start->first );
          size_t count = 0;
          
          for ( RevRefMap::iterator cur = start; cur != end; ++cur )
          {
            *sStr << "  " << count << " ";
            pni::pstd::dbgRefCount::doDbg ( start->second );
            ++count;
          }
          
          count = 0;
          start = end;
          end = sRevRefs.upper_bound ( start->first );
        }
      }
      
  void printOrphans ()
      {
        *sStr << " orphan report ( " << sOrphans.size () << " total )" << std::endl;
      
        RefSet::iterator end = sOrphans.end ();
        for ( RefSet::iterator cur = sOrphans.begin ();  cur != end; ++cur )
        {
          *sStr << "  ";
          dbgRefCount::doDbg ( *cur );
        }
      }
  
  void doExhaustiveDbg ()
      {
          // Initialize orphans to all refs... then remove refs
          // as they are encountered... the left over ones will 
          // be orphans.
        sOrphans = sRefs;
      
        RefSet::iterator setEnd = sRefs.end ();
        for ( RefSet::iterator setCur = sRefs.begin ();
            setCur != setEnd;  ++setCur )
        {
            // Collect the refs.
          Refs refs;
          dbgRefCount::collectRefs ( *setCur, refs );
          
            // Add to the fwd ref list.  Will likely contain nulls.
          sFwdRefs[ *setCur ] = refs;
          
            // Add to the rev ref list.
          Refs::iterator end = refs.end ();
          for ( Refs::iterator cur = refs.begin ();
              cur != end; ++cur )
          {
              // Protect against the common nulls in the fwd ref vectors.
            if ( *cur )
            {
              sOrphans.erase ( *cur );
            
                // Had to use a val object here... other overload
                // for insert was puking in msvc.
              RevRefMap::value_type val ( *cur, *setCur );
              sRevRefs.insert ( val );
            }
          }
        }
        
        printRevRefs ();
        printOrphans ();
        
        sFwdRefs.clear ();
        sRevRefs.clear ();
        sOrphans.clear ();
      }
}

////////

namespace pni {
	namespace pstd {

void dbgRefCount::doDbg ( unsigned int mask )
{
  if ( mask & dbgRefCount::Summary )
  {
    *sStr << "dbgRefCount::doDbg:" << std::endl;
    *sStr << " total entries: " << ( unsigned int ) sRefs.size () << std::endl;
  }
 
  typedef std::map< std::string, size_t > RefMap;
  RefMap sRefHisto;
  
  RefSet::iterator end = sRefs.end ();
  for ( RefSet::iterator cur = sRefs.begin (); cur != end; ++cur )
  {
    if ( mask & dbgRefCount::Details )
      ( *cur )->doDbg ();
    
    Ref* pRef = *cur;
    
    std::string name = typeid ( *pRef ).name ();
    
    RefMap::iterator found = sRefHisto.find ( name );
    if ( found != sRefHisto.end () )
      ++found->second;
    else
      sRefHisto[ name ] = 1;
  }
  
  if ( mask & dbgRefCount::Summary )
  {
    RefMap::iterator mend = sRefHisto.end ();
    for ( RefMap::iterator cur = sRefHisto.begin (); cur != mend; ++cur )
    {
      *sStr << "  " << cur->second << " occurrences of " << cur->first << std::endl;
    }
  }
  
  if ( mask & Exhaustive )
  {
    doExhaustiveDbg ();
  }
}

void dbgRefCount::doDbg ( Ref* pRef )
{
  pRef->doDbg ();
}

void dbgRefCount::setStr ( std::ostream* str )
{
  sStr = str;
}

std::ostream* dbgRefCount::getStr ()
{
  return sStr;
}

void dbgRefCount::trackRef ( Ref* pRef )
{
  sTrackRef = pRef;
}

void dbgRefCount::collectRefs ( Ref* pRef, Refs& refs )
{
  pRef->collectRefs ( refs );
}

  } // setEnd of namespace pstd
} // setEnd of namespace pni

#else //////////////////////////////////////////////////////////////

namespace 
{
  void dbgNewRef ( Ref* pRef ) {}
  void dbgDelRef ( Ref* pRef ) {}

  void dbgRef ( Ref* pRef ) {}
  void dbgUnref ( Ref* pRef ) {}
}

////////

namespace pni {
	namespace pstd {

void dbgRefCount::doDbg ( unsigned int mask ) {}
void dbgRefCount::setStr ( std::ostream* str ) {}
std::ostream* dbgRefCount::getStr () { return 0; }
void dbgRefCount::trackRef ( Ref* pRef ) {}
void dbgRefCount::collectRefs ( Ref* pRef, Refs& refs ) {}
void dbgRefCount::doDbg ( Ref* pRef ) {}

  } // setEnd of namespace pstd
} // setEnd of namespace pni

#endif  // PNIREFCOUNTDBG

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

refCount::
refCount () :
	refs ( 0 )
{
	dbgNewRef ( this );
}

refCount::
refCount ( refCount const& rhs ) :
	refs ( 0 )
{
  dbgNewRef ( this );
}

refCount::
~refCount ()
{
	assert ( refs == 0 );

  dbgDelRef ( this );
}

refCount&
refCount::
operator = ( refCount const& rhs )
{
    // Specifically doesn't copy rhs refcount!!!
	return *this;
}

/////////////////////////////////////////////////////////////////////


bool
refCount::
ref ( int32 howMany ) const
{
//	platform::pstd::atomicAdd ( &refs, howMany );
	refs += howMany;
	dbgRef ( this );
	assert ( refs >= 0 );
	return true;
}

bool
refCount::
unref ( int32 howMany ) const
{
//	if ( platform::pstd::atomicAdd ( &refs, -howMany ) == 1 )
	refs -= howMany;
  dbgUnref ( this );
	if ( refs <= 0 )
	{
		delete this;
		return true;
	}
	else
	{
		assert ( refs > 0 );
		return false;
	}
}

refCount::int32
refCount::
getNumRefs () const
{
		return refs;
//	return platform::pstd::atomicGet ( &refs );
}

/////////////////////////////////////////////////////////////////////

void refCount::doDbg () const
{
#ifdef PNIREFCOUNTDBG
  *sStr << "  " << this << " is ref'd " << refs << " times "
      << typeid ( *this ).name ();
  
  onDoDbg ();
  
  *sStr << std::endl;
#else

#endif
}

/////////////////////////////////////////////////////////////////////


	} // setEnd of namespace pstd 
} // setEnd of namespace pni 


