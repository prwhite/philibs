/////////////////////////////////////////////////////////////////////
//
//    class: textureXform
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenetexturexform_h
#define scenescenetexturexform_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"
#include "pnimatrix4.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class textureXform :
  public state
{
  public:
    textureXform();
    virtual ~textureXform();
//     textureXform(textureXform const& rhs);
//     textureXform& operator=(textureXform const& rhs);
//     bool operator==(textureXform const& rhs) const;
    
    pni::math::matrix4& operator * () { return mMat; }
    pni::math::matrix4 const& getMatrix () const { return mMat; }
    
  protected:
    
  private:
    pni::math::matrix4 mMat;

  // interface from state
  public:
    virtual state* dup () const { return new textureXform ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenetexturexform_h


