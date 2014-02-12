/////////////////////////////////////////////////////////////////////
//
//    class: converter
//
//    Really doesn't have much state... just a convenient place
//    to put conversions from different frames.
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneconverter_h
#define scenesceneconverter_h

/////////////////////////////////////////////////////////////////////

#include "scenenode.h"
#include "scenecamera.h"
#include "scenenodepath.h"
#include "scenegraphdd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  class camera;
  
/////////////////////////////////////////////////////////////////////

class converter :
  protected graphDd
{
  public:
    converter();
//     virtual ~converter();
//     converter(converter const& rhs);
//     converter& operator=(converter const& rhs);
//     bool operator==(converter const& rhs) const;
    
    void getConvertMatrix ( pni::math::matrix4& dst,
        nodePath const& dstPath, nodePath const& srcPath );
    
      // dstPath must be to a camera node.
    bool getWorldToScreenMatrix ( pni::math::matrix4& dst,
        nodePath const& dstPath, nodePath const& srcPath );
      
      // dstPath must be to a camera node.
      // this does everything but the viewport.
    bool getWorldToCamMatrix  ( pni::math::matrix4& dst,
        nodePath const& dstPath, nodePath const& srcPath );

      // Convert between two node coordinate frames.
    void convert ( pni::math::vec3& dst, pni::math::vec3 const& src,
        nodePath const& dstPath, nodePath const& srcPath );
    
      // Note that dstPath should be a path to a camera node.
    void worldToScreen ( pni::math::vec3& dst, pni::math::vec3 const& src,
        nodePath const& dstPath, nodePath const& srcPath );

      // Note that srcPath should be a path to a camera node.
    void screenToWorld ( pni::math::vec3& dst, pni::math::vec3 const& src,
        nodePath const& dstPath, nodePath const& srcPath );

  protected:
    
    virtual void startGraph ( node const* pNode ) {}
    
  protected:
    
    virtual void dispatch ( camera const* pNode );
    virtual void dispatch ( geom const* pNode ) {}
    virtual void dispatch ( geomFx const* pNode ) {};
    virtual void dispatch ( group const* pNode ) {}
    virtual void dispatch ( light const* pNode ) {}
    virtual void dispatch ( sndEffect const* pNode ) {}
    virtual void dispatch ( sndListener const* pNode ) {}

    void collectRefs ( pni::pstd::refCount::Refs& refs ) const {}

  private:
    camera const* mCam;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneconverter_h


