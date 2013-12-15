/////////////////////////////////////////////////////////////////////
//
//    class: matrixHelper
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimmatrixhelper_h
#define animanimmatrixhelper_h

/////////////////////////////////////////////////////////////////////

#include "pnimatrix4.h"
#include "pnivec3.h"
#include "pnivec4.h"
#include "pniquat.h"
#include "pnirefcount.h"

#include "animidbase.h"

/////////////////////////////////////////////////////////////////////

namespace anim {

/////////////////////////////////////////////////////////////////////

class matrixHelper :
  public pni::pstd::refCount,
  public idBase
{
  public:
    matrixHelper();
//     virtual ~matrixHelper();
//     matrixHelper(matrixHelper const& rhs);
//     matrixHelper& operator=(matrixHelper const& rhs);
//     bool operator==(matrixHelper const& rhs) const;
    
      ///////
      // Transformation routines.

      // Call updateMatrix when you need the current matrix value...
      // this will do lazy update of any recent values
    pni::math::matrix4& updateMatrix ();

    enum RotMode
    {
      Undef,
      AngleAxis,
      Quat
    };
    
    void setRotMode ( RotMode rotMode ) { mRotMode = rotMode; }
    RotMode getRotMode () const { return mRotMode; }
    
    pni::math::vec3& transOp () { mDirty = true; return mTrans; }
    pni::math::vec3& scaleOp () { mDirty = true; return mScale; }
    pni::math::vec4& angleAxisOp () { mDirty = true; return mAngleAxis; }
    pni::math::quat& quatOp () { mDirty = true; return mQuat; }
    
  protected:
        
    pni::math::vec3 mTrans;
    pni::math::vec3 mScale;
    pni::math::vec4 mAngleAxis;
    pni::math::quat mQuat;
   
    pni::math::matrix4 mMat;
    RotMode mRotMode;
    
    bool mDirty;
    
  private:

  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimmatrixhelper_h


