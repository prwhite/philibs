/////////////////////////////////////////////////////////////////////
//
//    class: coreImage
//
/////////////////////////////////////////////////////////////////////

#ifndef imgimgcoreimage_h
#define imgimgcoreimage_h

/////////////////////////////////////////////////////////////////////

#include "imgbase.h"
#include <string>

/////////////////////////////////////////////////////////////////////

namespace img {
  
/////////////////////////////////////////////////////////////////////

class coreImage
{
  protected:
    virtual ~coreImage();

  public:
  	typedef pni::pstd::autoRef< img::base > ImgRef;

    coreImage();
//     coreImage(coreImage const& rhs);
//     coreImage& operator=(coreImage const& rhs);
//     bool operator==(coreImage const& rhs) const;
    
    
    static base* loadHelper ( std::string const& fname );
    
  protected:

    base* mImg;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace img 

#endif // imgimgcoreimage_h


