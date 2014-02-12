/////////////////////////////////////////////////////////////////////
//
//  class: trans
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimtrans_h
#define animanimtrans_h

/////////////////////////////////////////////////////////////////////

#include "animbase.h"
#include "animmatrixhelper.h"
#include "animmanager.h"
#include "animcommon.h"

#include "pnivec3.h"

#include <vector>


/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

class trans :
  public base,
  public matrixHelperBase
{
  public:
    //trans ();
    //virtual ~trans ();
    //trans ( trans const& rhs );
    //trans& operator= ( trans const& rhs );
    //bool operator== ( trans const& rhs ) const;


      // TODO: These keys are too primitive.  They need to also
      // include timestamp, TCB and ease-in/out values (for Max support).
      
      // TODO: These keys should be in ref-counted class to support
      // cheap aliasing of animation data a la scene::geomData.
    typedef std::vector< pni::math::vec3 > Keys;
    Keys mKeys;
    
    void initStartFromCur ();
    void addKeyWithOffset ( pni::math::vec3 const& val )
        {
          pni::math::vec3 const& src = mKeys.back ();
          mKeys.push_back ( src + val );
        }
        
    virtual scene::matrix4& matrixOp () = 0;
    virtual scene::matrix4 const& getMatrix () const = 0;
        
  protected:
      void initId ();

  private:


    // interface from base
  public:
    virtual void internalUpdate ( manager* pManager, TimeType tval );
    virtual void finalize () = 0;
    virtual bool needsFinalize () const { return true; }

  protected:

    // interface from matrixHelperNode
  public:
    virtual void setMatrixHelper ( matrixHelper* pHelper ) 
        { 
          matrixHelperBase::setMatrixHelper ( pHelper );
          initId ();
        }

  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimtrans_h


