/////////////////////////////////////////////////////////////////////
//
//  file: pniscenenodepath.cpp
//
/////////////////////////////////////////////////////////////////////

#include "scenenodepath.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

nodePath::
nodePath () :
  mDoRef ( true )
{
  
}

//nodePath::
//nodePath ( const nodePath& orig )
//{
//  
//}

// nodePath::
// ~nodePath ()
// {
//   
// }

//nodePath&
//nodePath::
//operator = ( const nodePath& orig )
//{
//  
//  return *this;
//}

/////////////////////////////////////////////////////////////////////

void nodePath::ref ( node* pNode )
{
  if ( mDoRef )
    pNode->ref ();
}

void nodePath::unref ( node* pNode )
{
  if ( mDoRef )
    pNode->unref ();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

bool nodePath::find ( nodePath& path, node* start, std::string const& dst, bool down )
{
  path.push ( start );
  
  if ( start->getName () == dst )
    return true;
  
  if ( down )
  {
    typedef node::Nodes::iterator Iter;
    Iter end = start->getChildren ().end ();
    for ( Iter cur = start->getChildren ().begin (); cur != end; ++cur )
    {
      if ( find ( path, cur->get (), dst, down ) )
        return true;
      else
        path.pop ();
    }
  }
  else
  {
    typedef node::Parents::iterator Iter;
    Iter end = start->getParents ().end ();
    for ( Iter cur = start->getParents ().begin (); cur != end; ++cur )
    {
      if ( find ( path, *cur, dst, down ) )
        return true;
      else
        path.pop ();
    }
  }
  
  return false;
}

bool nodePath::find ( nodePath& path, node* start, node* dst, bool down )
{
  path.push ( start );
  
  if ( start == dst )
    return true;
  
  if ( down )
  {
    typedef node::Nodes::iterator Iter;
    Iter end = start->getChildren ().end ();
    for ( Iter cur = start->getChildren ().begin (); cur != end; ++cur )
    {
      if ( find ( path, cur->get (), dst, down ) )
        return true;
      else
        path.pop ();
    }
  }
  else
  {
    typedef node::Parents::iterator Iter;
    Iter end = start->getParents ().end ();
    for ( Iter cur = start->getParents ().begin (); cur != end; ++cur )
    {
      if ( find ( path, *cur, dst, down ) )
        return true;
      else
        path.pop ();
    }
  }
  
  return false;
}

/////////////////////////////////////////////////////////////////////

bool nodePath::contains ( node* pNode ) const
{
  Nodes::const_iterator found = std::find ( mNodes.begin (), mNodes.end (), pNode );
  
  return ( found != mNodes.end () );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void nodePath::collectRefs ( Refs& refs )
{
  if ( mDoRef )
  {

    Nodes::iterator end = mNodes.end ();
    for ( Nodes::iterator cur = mNodes.begin (); cur != end; ++cur )
    {
      refs.push_back ( *cur );
    }
  }
}

/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


