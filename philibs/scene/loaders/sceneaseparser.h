/////////////////////////////////////////////////////////////////////
//
//  class: parser
//
/////////////////////////////////////////////////////////////////////

#ifndef SCENEASEASEPARSER_H
#define SCENEASEASEPARSER_H

/////////////////////////////////////////////////////////////////////

// #include "pnisceneasedef.h"
#define SCENEASEAPI

#include "pniplatform.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <fstream>

#include <cassert>
#include <cstdlib>

#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>

/////////////////////////////////////////////////////////////////////

namespace ase {

/////////////////////////////////////////////////////////////////////

class str
{
    std::string mData;
    size_t mCurEol;
    size_t mCurSol;
    size_t mLen;
    bool mGood;

    size_t findNextEol ( size_t startPos )
        {
          return mData.find_first_of ( "\n\r", startPos );
        }
        
    size_t findNextSol ( size_t startPos )
        {
          return mData.find_first_not_of ( "\n\r", startPos );
        }

  public:
    str ( std::string const& fname ) :
      mCurEol ( std::string::npos ),
      mCurSol ( std::string::npos ),
      mLen ( std::string::npos ),
      mGood ( false )
        {
          std::ifstream istr ( fname.c_str () );
          if ( istr.good () )
          {
#ifdef PNI_TARGET_WIN32
            struct _stat tstat;
            _stat ( fname.c_str (), &tstat );
#else
            struct stat tstat;
            stat ( fname.c_str (), &tstat );
#endif
            mLen = ( size_t ) tstat.st_size;
            
            mData.resize ( mLen + 16 );
            
            istr.read ( &mData[ 0 ], ( std::streamsize ) mLen );
            
            mGood = true;

            mCurSol = 0;            
            mCurEol = findNextEol ( mCurSol );
          }
        }
        
    bool good () const
        {
          return mGood;
        }
    
    bool eof () const
        {
          return mCurEol == std::string::npos;
        }
    
    size_t getline ( std::string& str )
        {
//assert ( mCurEol != std::string::npos );

          if ( mCurEol == std::string::npos )
            return 0;

          str = mData.substr ( mCurSol, mCurEol - mCurSol );

//printf ( "%s\n", str.c_str () );
          
          size_t ret = mCurEol - mCurSol - 1;

          mCurSol = findNextSol ( mCurEol );
          mCurEol = findNextEol ( mCurSol );
          
          if ( mCurEol == std::string::npos )
          {
            mGood = false;
            return 0;
          }
          else
          {
            return ret;
          }
        }
};

/////////////////////////////////////////////////////////////////////

inline
size_t getline ( str& istr, std::string& str )
    {
      return istr.getline ( str );
    }

/////////////////////////////////////////////////////////////////////

class SCENEASEAPI line
{
  public:
  
  typedef str Stream;
  //typedef std::ifstream Stream;
  
  line ( const std::string& str ) :
    mString ( str )
    {
      setName ();
    }
  
  line ( Stream& istr )
    {
      using namespace std;
      if ( istr.good () )
      getline ( istr, mString );

      setName ();
    }
  
  void setString ( const std::string& str ) { mString = str; setName (); }
  const std::string& getString () const { return mString; }
  
  void setName ()
    {
      size_t start = mString.find ( '*' );
      if ( start != std::string::npos )
      {
        ++start;
        
        size_t end = mString.find_first_of ( " \t\n\r", start );
        if ( end != std::string::npos )
        {
          if ( end - start <= 0 )
          {
            std::cout << "e : s " << ( int ) end << " : " << ( int ) start << std::endl;
          }
          assert ( end - start > 0 );
          mName = mString.substr ( start, end - start );
  //        ++end;
          if ( end < ( int ) mString.size () )
          {
  // one and only one tag, BITMAP_INVERT, does not have an
  // associated value... so it doesn't have one extra char for
  // its name.
      end = end < mString.size () ? end + 1 : end;
  //          assert ( end + 1 < ( int ) mString.size () );
//          mString = mString.substr ( end ); // Really?
          
            // This might be faster than the above substr call.
          mString.replace ( 0, 10000, mString, end, 10000 );
          }
        }
      }
    }
    
  const std::string& getName () const { return mName; }

  bool isPush () const
    {
      return ( mString.find_first_of ( '{' ) != std::string::npos );
    }
    
  bool isPop () const
    {
      return ( mString.find_last_of ( '}' ) != std::string::npos );
    }
  
  // Parsing helper methods.
  int getInt () const
    {
      int tmp = 0;
      sscanf ( mString.c_str (), "%d", &tmp );
      return tmp;
    }
    
  float strtof ( char const* src, char** ptr ) const
      {
        return ( float ) ( ::strtod ( src, ptr ) );
      }
      
  int strtoi ( char const* src, char** ptr ) const
      {
        return ( int ) ::strtol ( src, ptr, 10 );
      }
    
  float getFloat () const
    {
#ifdef ASEUSESCANF
      float tmp = 0.0f;
      sscanf ( mString.c_str (), "%f", &tmp );
      return tmp;
#else
      return strtof ( mString.c_str (), 0 );
#endif
    }

  void getInt3 ( int vec[ 3 ] ) const
    {
#ifdef ASEUSESCANF
      sscanf ( mString.c_str (), "%d %d %d", &vec[ 0 ], &vec[ 1 ], &vec[ 2 ] );
#else
      char* end = ( char* )  mString.c_str ();
      vec[ 0 ] = strtoi ( end, &end );
      vec[ 1 ] = strtoi ( end, &end );
      vec[ 2 ] = strtoi ( end, 0 );
#endif
    }
  
  void getFloat3 ( float vec[ 3 ] ) const
    {
#ifdef ASEUSESCANF
      sscanf ( mString.c_str (), "%f %f %f", &vec[ 0 ], &vec[ 1 ], &vec[ 2 ] );
#else
      char* end = ( char* ) mString.c_str ();
      vec[ 0 ] = strtof ( end, &end );
      vec[ 1 ] = strtof ( end, &end );
      vec[ 2 ] = strtof ( end, 0 );
#endif
    }

  void getIntInt3 ( int& ind, int vec[ 3 ] ) const
    {
#ifdef ASEUSESCANF
      sscanf ( mString.c_str (), "%d %d %d %d", &ind, &vec[ 0 ], &vec[ 1 ], &vec[ 2 ] );
      char* end = ( char* )  mString.c_str ();
#else
      char* end = ( char* ) mString.c_str ();
      ind = strtoi ( end, &end );
      vec[ 0 ] = strtoi ( end, &end );
      vec[ 1 ] = strtoi ( end, &end );
      vec[ 2 ] = strtoi ( end, 0 );
#endif
    }
  
  void getIntFloat2 ( int& ind, float vec[ 2 ] ) const
    {
#ifdef ASEUSESCANF
      sscanf ( mString.c_str (), "%d %f %f", &ind, &vec[ 0 ], &vec[ 1 ] );
#else
      char* end = ( char* ) mString.c_str ();
      ind = strtoi ( end, &end );
      vec[ 0 ] = strtof ( end, &end );
      vec[ 1 ] = strtof ( end, &end );
#endif
    }

  void getIntFloat3 ( int& ind, float vec[ 3 ] ) const
    {
#ifdef ASEUSESCANF
      sscanf ( mString.c_str (), "%d %f %f %f", &ind, &vec[ 0 ], &vec[ 1 ], &vec[ 2 ] );
#else
      char* end = ( char* ) mString.c_str ();
      ind = strtoi ( end, &end );
      vec[ 0 ] = strtof ( end, &end );
      vec[ 1 ] = strtof ( end, &end );
      vec[ 2 ] = strtof ( end, &end );
#endif
    }

  std::string getSymbol () const
    {
      const unsigned int Max = 256;
      char tmp[ Max ];
      sscanf ( mString.c_str (), "%256[^\"]", tmp );
      std::string retVal ( tmp );
      return retVal;
    }
      
  std::string getQuoted () const
    {
      const unsigned int Max = 256;
      char tmp[ Max ];
      sscanf ( mString.c_str (), "\"%256[^\"]", tmp );
      std::string retVal ( tmp );
      return retVal;
    }
    
  void getMeshFace ( int& ind, int vec[ 3 ] ) const
    {
      sscanf ( mString.c_str (), "%d: A: %d B: %d C: %d", 
        &ind, &vec[ 0 ], &vec[ 1 ], &vec[ 2 ] );
    }
  
  
  private:
  std::string mString;
  std::string mName;
};

/////////////////////////////////////////////////////////////////////

class SCENEASEAPI node :
  public line
{
  public:
  typedef std::vector< line* > Lines;
  typedef Lines::iterator LineIter;
  typedef Lines::const_iterator ConstLineIter;
  typedef std::vector< node* > Nodes;
  typedef Nodes::iterator NodeIter;
  typedef Nodes::const_iterator ConstNodeIter;
  typedef std::multimap< std::string, line* > LineMap;
  typedef LineMap::iterator LineMapIter;
  typedef LineMap::const_iterator ConstLineMapIter;
  
  node ( const std::string& str ) :
    line ( str )
    {
    }
    
  node ( Stream& istr ) :
    line ( istr )
    {
    }
    
  node ( const line* pLine ) :
    line ( *pLine )
    {
    }
    
  ~node ()
    {
      Nodes::iterator end = mKids.end ();
      for ( Nodes::iterator cur = mKids.begin (); cur != end; ++cur )
        delete *cur;
      
      LineIter lend = mLines.end ();
      for ( LineIter cur = mLines.begin (); cur != lend; ++cur )
        delete *cur;
    }
  
    void pushLine ( line* pLine )
      {
        mLines.push_back ( pLine );
      }
    
    void pushChild ( node* pNode )
      {
        mKids.push_back ( pNode ); 
      }
  
    void makeMap ()
      {
      LineIter end = mLines.end ();
      for ( LineIter cur = mLines.begin (); cur != end; ++cur )
      {
std::cout << "line name = " << ( *cur )->getName () << std::endl;
        mLineMap.insert ( LineMap::value_type ( ( *cur )->getName (), *cur ) );
      }
      }
    
    line const* findLine ( const std::string& name, bool orElse = true ) const
      {
        ConstLineIter end = mLines.end ();
        for ( ConstLineIter cur = mLines.begin (); cur != end; ++cur )
          if ( ( *cur )->getName () == name )
            return *cur;
      
// ConstLineMapIter end = mLineMap.end ();
// for ( ConstLineMapIter cur = mLineMap.begin (); cur != end; ++cur )
//   std::cout << "line name = " << cur->second->getName () << std::endl;
// 
// std::cout << "size of mLineMap = " << mLineMap.size () << std::endl;
// 
//       ConstLineMapIter found = mLineMap.find ( name );
//       if ( found != mLineMap.end () )
//         return found->second;
//       else
//       {
//         if ( orElse )
// #if __EXCEPTIONS==1
//         throw;
//         else
// #endif // __EXCEPTIONS
        return 0;
//         }
      }
      
    node const* findNode ( const std::string& name, bool orElse = true ) const
      {
      ConstNodeIter end = mKids.end ();
      for ( ConstNodeIter cur = mKids.begin (); cur != end; ++cur )
      {
        if ( ( *cur )->getName () == name )
        return *cur;
      }
      
#if __EXCEPTIONS==1
      if ( orElse )
        throw;
#endif // __EXCEPTIONS
      return 0;
      }
      
    LineIter getLineBegin () { return mLines.begin (); }
    LineIter getLineEnd () { return mLines.end (); }
    
    NodeIter getChildBegin () { return mKids.begin (); }
    NodeIter getChildEnd () { return mKids.end (); }
    
    LineMapIter getLineMapBegin () { return mLineMap.begin (); }
    LineMapIter getLineMapEnd () { return mLineMap.end (); }

    ConstLineIter getLineBegin () const { return mLines.begin (); }
    ConstLineIter getLineEnd () const { return mLines.end (); }
    
    ConstNodeIter getChildBegin () const { return mKids.begin (); }
    ConstNodeIter getChildEnd () const { return mKids.end (); }
    
    ConstLineMapIter getLineMapBegin () const { return mLineMap.begin (); }
    ConstLineMapIter getLineMapEnd () const { return mLineMap.end (); }
  
  private:
  Lines mLines;
  Nodes mKids;
  LineMap mLineMap;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class SCENEASEAPI parser
{
  public:
    typedef line::Stream Stream;

    parser ();
    virtual ~parser ();
    
    node* parse ( const std::string& fname );
    node* parse ( Stream& istr );
    
  protected:
    
    void parseNode ( Stream& istr, node* pCur );
  
  private:
    parser ( const parser& orig );
    parser& operator = ( const parser& orig );
    
    int mDepth;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace ase 

#endif // SCENEASEASEPARSER_H


