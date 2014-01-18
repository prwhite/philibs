/////////////////////////////////////////////////////////////////////
//
//  file: pnisceneaseparser.cpp
//
/////////////////////////////////////////////////////////////////////

#include "sceneaseparser.h"

#include <fstream>

/////////////////////////////////////////////////////////////////////

namespace ase {
  
/////////////////////////////////////////////////////////////////////

parser::parser () :
  mDepth ( 0 )
{
  
}

parser::~parser ()
{
  
}

//parser::parser ( const parser& orig )
//{
//  
//}
//
//parser& parser::operator = ( const parser& orig )
//{
//  
//  return *this;
//}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void parser::parseNode ( Stream& istr, node* pCur )
{
  ++mDepth;
  
  bool done = false;
  
  while ( ! done )
  {
    if ( istr.eof () )
      break;
  
    line* cur = new line ( istr );

    if ( cur->isPush () )
    {
      node* pNode = new node ( cur );
      delete cur;
      parseNode ( istr, pNode );

      if ( pNode->getName () != "" )
        pCur->pushChild ( pNode );
      else
        delete pNode;
    }
    else if ( cur->isPop () )
    {
  //   pCur->pushLine ( cur );
      done = true;
      delete cur;
    }
    else
    {
      pCur->pushLine ( cur );
    }

    if ( istr.eof () )
      done = true;
  }
  
//   pCur->makeMap (); // After it's been filled up, so the vector doesn't realloc and
      // kill the pointers in the map.  HACK.
  
  --mDepth;
}

node* parser::parse ( const std::string& fname )
{
  //str tstr ( fname );
  //std::string tmp;
  //tstr.getline ( tmp );
  //tstr.getline ( tmp);

  Stream istr ( fname.c_str () );
  
  if ( istr.good () )
    return parse ( istr );
  else
    return 0;
}

node* parser::parse ( Stream& istr )
{
  if ( ! istr.good () )
  return 0;

  node* pRoot = new node ( istr );

  parseNode ( istr, pRoot );

  return pRoot;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

} // end of namespace ase 


