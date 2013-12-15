/////////////////////////////////////////////////////////////////////
//
//  class: flattenDd
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneflattendd_h
#define scenesceneflattendd_h

/////////////////////////////////////////////////////////////////////

#include "scenegraphdd.h"

#include "pnimatstack.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class flattenDd :
  public scene::graphDd
{
  public:
    flattenDd ();
    //virtual ~flattenDd ();
    //flattenDd ( flattenDd const& rhs );
    //flattenDd& operator= ( flattenDd const& rhs );
    //bool operator== ( flattenDd const& rhs ) const;

      // Values for opts
    enum
    {
      None = 0x00,
      LeaveCenterXform = 0x01,
      LeaveCenterAtOrigin = 0x02,
      Renormalize= 0x04
    };

      // Default opts is == None.
    void setFlattenOpts ( unsigned int opts ) { mOpts = opts; }
    unsigned int getFlattenOpts () const { return mOpts; }

  protected:

  private:
    void dispatchChildren ( node const* pNode );
    void dispatchPre ( node const* pNode );
    void dispatchPost ( node const* pNode );
  
    pni::math::matStack< pni::math::matrix4 > mMatStack;
    unsigned int mOpts;
    size_t mLevel;

    // interface from graphDd
  public:

      // graphDd framework:
    virtual void startGraph ( node const* pNode );

  protected:
    virtual void dispatch ( camera const* pNode );
    virtual void dispatch ( geom const* pNode );
    virtual void dispatch ( group const* pNode );
    virtual void dispatch ( light const* pNode );
    virtual void dispatch ( geomFx const* pNode );
    virtual void dispatch ( sndEffect const* pNode );
    virtual void dispatch ( sndListener const* pNode );
};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneflattendd_h


