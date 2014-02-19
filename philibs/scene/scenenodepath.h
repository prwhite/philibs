/////////////////////////////////////////////////////////////////////
//
//  class: nodePath
//  
//  This class has a real issue with const-ness.  E.g., in graph
//  traversal the mNodes are all const, so they can't be pushed
//  into a nodePath instance.  This should probably be solved
//  by switching this to a template class.  
//
//  Also, while we're at it, the class could use a helper template
//  member function that will do a static or dynamic cast to the
//  explicitly requested destination type.
//
/////////////////////////////////////////////////////////////////////

#ifndef pniscenenodepath_h
#define pniscenenodepath_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include <vector>
#include <algorithm>
#include "pniautoref.h"
#include "scenenode.h"
#include "pnimatrix4.h"

#include "pnirefcountdbg.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class nodePath 
{
    enum MagicNumbers { 
        Reserve = 8
    }; // Magic Number!

  public:
    nodePath ();
    explicit nodePath ( bool doRef ) : mDoRef ( doRef ) {}
    nodePath ( node* nodeIn ) : mDoRef ( true ) { init ( nodeIn ); }
    nodePath ( const nodePath& orig ) :
      mDoRef ( true )
        {
          mNodes = orig.mNodes;
          Iter end = mNodes.rend ();
          for ( Iter cur = mNodes.rbegin (); cur != end; ++cur )
            ref ( *cur );
        }

     ~nodePath ()
        {
          clear ();
        }
        
    nodePath& operator = ( const nodePath& orig )
        {
          if ( &orig == this )
            return *this;
        
          clear ();
          mNodes = orig.mNodes;
          Iter end = mNodes.rend ();
          for ( Iter cur = mNodes.rbegin (); cur != end; ++cur )
            ref ( *cur );
            
          return *this;
        }
    
    // Make a path from a node.  Returns false if there is any
    // multiple-parantage up the graph or if node is null.
    // acts like 'clear' if node is null.
    bool init ( node* nodeIn )
        {
          clear ();
          mNodes.reserve ( Reserve );
          
          bool retVal = true;
  
          if ( ! nodeIn )
            return false;
  
          push ( nodeIn );
          
          while ( size_t num = nodeIn->getParents ().size () )
          {
            // is there multiple-parantage?
            if ( num > 1 )
              retVal = false;

            nodeIn = nodeIn->getParents ()[ 0 ];
            push ( nodeIn );
          }
          
          return retVal;
        }

    void clear () 
        { 
          Iter end = mNodes.rend ();
          for ( Iter cur = mNodes.rbegin (); cur != end; ++cur )
            unref ( *cur );
        
          mNodes.clear (); 
        }

    void getPathString ( std::string& strOut ) const
        {   
          strOut.erase ();

          for ( ConstIter iter = mNodes.rbegin ();
              iter != mNodes.rend ();
              ++iter )
          {
            strOut.append ( "/" );
            strOut.append ( ( *iter )->getName () );
          }
        }

    void push ( node* nodeIn )
        {
          ref ( nodeIn );
          mNodes.push_back ( nodeIn );
        }
    
    void pop ()
        {
          unref ( mNodes.back () );
          mNodes.pop_back ();
        }   

    // OPT: cache this calc in a stack so it can be retrieved
    // more cheaply.
    void calcXform ( pni::math::matrix4& mat ) const
        {
          mat.setIdentity ();

          for ( ConstIter iter = mNodes.rbegin ();
              iter != mNodes.rend ();
              ++iter )
          {
#define NODEPATHOPTIMIZATION
#ifdef NODEPATHOPTIMIZATION
            if ( ! ( *iter )->getMatrix ().getIsIdent () )
#endif // NODEPATHOPTIMIZATION
              mat.preMult ( ( *iter )->getMatrix () );  
          } 
        }

    // OPT: only generate this when the corresponding fwd mat
    // has changed.
    void calcInverseXform ( pni::math::matrix4& mat ) const
        {
          calcXform ( mat );
          mat.invert ();
        }

      // Note: path should be empty before calling these methods.
      // GOTCHA: By convention... the leaf is .front... so when going
      // down, reverse should be called.
    static bool find ( nodePath& path, node* start, std::string const& dst, bool down );
    static bool find ( nodePath& path, node* start, node* dst, bool down );

    node* operator [] ( int which ) { return mNodes[ which ]; };
    const node* operator [] ( int which ) const { return mNodes[ which ]; };
    size_t getSize () const { return mNodes.size (); }
    node* get ( int which ) const { return mNodes[ which ]; };
    node* getRoot () const { if ( ! mNodes.empty () ) return mNodes.back (); else return 0; }
    node* getLeaf () const { if ( ! mNodes.empty () ) return mNodes.front (); else return 0; }
    template< class Type >
    Type* getLeaf () const { if ( ! mNodes.empty () ) 
        return static_cast< Type* > ( mNodes.front () ); else return 0; }
    bool empty () const { return mNodes.empty (); }
    bool contains ( node* pNode ) const;
    
    void reverse () { std::reverse ( mNodes.begin (), mNodes.end () ); }

    //typedef std::vector< pni::pstd::autoRef< node > > Nodes;
    typedef std::vector< node* > Nodes;
    typedef Nodes::reverse_iterator Iter;
    typedef Nodes::const_reverse_iterator ConstIter;

    Nodes& getNodes () { return mNodes; }
    Nodes const& getNodes () const { return mNodes; }

      // Do/don't use ref counting for contained mNodes.
      // Do not change after adding entries!!!
    void setDoRef ( bool val ) { mDoRef = val; }
    bool getDoRef () const { return mDoRef; }

      // Just used for debugging refCount issues.
    typedef pni::pstd::dbgRefCount::Refs Refs;
    
    void collectRefs ( Refs& refs ) const;

  protected:
    void ref ( node* pNode );
    void unref ( node* pNode );
  

  private:

    Nodes mNodes;
    bool mDoRef;
};

  // Can be used kinda transparently in classes that want the nodePath
  // to default to non-referencing.  This is handing if you are keeping
  // a std::vector of nodePaths that should not reference.  The default
  // initialization will do the right thing.
class nodePathNoRef :
  public nodePath
{
  public:
    nodePathNoRef () :
      nodePath ( false )
        {}

    nodePathNoRef ( nodePathNoRef const& rhs ) :
      nodePath ( rhs )
        {
          setDoRef ( false );
        }

    nodePathNoRef ( node* nodeIn ) :
      nodePath ( false )
        {
          init ( nodeIn );
        }
        
    nodePathNoRef& operator = ( nodePath const& rhs )
        {
          nodePath::operator = ( rhs );
          return *this;
        }
};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // pniscenenodepath_h


