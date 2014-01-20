/////////////////////////////////////////////////////////////////////
//
//    class: pvr
//
/////////////////////////////////////////////////////////////////////

#ifndef imgimgpvr_h
#define imgimgpvr_h

/////////////////////////////////////////////////////////////////////

#include "imgbase.h"
#include <string>

/////////////////////////////////////////////////////////////////////

namespace img {
  
/////////////////////////////////////////////////////////////////////

class pvr
{
  protected:
    virtual ~pvr();

  public:
  	typedef pni::pstd::autoRef< img::base > ImgRef;

    pvr();
//     pvr(pvr const& rhs);
//     pvr& operator=(pvr const& rhs);
//     bool operator==(pvr const& rhs) const;
    
    
    static base* loadHelper ( std::string const& fname );
    
  protected:

    base* mImg;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace img 

#endif // imgimgpvr_h

