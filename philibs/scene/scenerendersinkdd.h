/////////////////////////////////////////////////////////////////////
//
//  class: renderSinkDd
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenerendersinkdd_h
#define scenescenerendersinkdd_h

/////////////////////////////////////////////////////////////////////

#include "scenerendersink.h"
#include "scenegraphdd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {

class renderSink;
class graphDd;
    
/////////////////////////////////////////////////////////////////////

  /** A class to iterate over a graph of renderSink instances.
    While the interface is generally virtual, it is intended to
    be a useful concrete class without the need for specialization.
  
  */
class renderSinkDd 
{
    friend class renderSink;

  public:
//    renderSinkDd();
//    virtual ~renderSinkDd();
//    renderSinkDd(renderSinkDd const& rhs);
//    renderSinkDd& operator=(renderSinkDd const& rhs);
//    bool operator==(renderSinkDd const& rhs) const;

      /// Traverse a graph of renderSinks, executing dependent rendering
      /// operations in order so that the final composition to the target
      /// framebuffer is correct and coherent.
    virtual void startGraph ( renderSink const* pSink, TimeType timeStamp );

  protected:

    virtual void dispatch ( renderSink const* pSink );
    virtual void dispatch ( renderSink const* pSink,
        texture::ImageId colorId,
        texture::ImageId depthId,
        texture::ImageId stencilId,
        bool fastBind = true );
  
    virtual void execGraphDd ( renderSink::graphDdSpec const& spec );
  
  private:
      TimeType mCurTimeStamp          = 0.0;
      TimeType mLastTimeStamp         = 0.0;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenerendersinkdd_h


