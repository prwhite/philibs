/////////////////////////////////////////////////////////////////////
//
//    class: assimp
//
/////////////////////////////////////////////////////////////////////

#ifndef sceneloaderloaderassimp_h
#define sceneloaderloaderassimp_h

/////////////////////////////////////////////////////////////////////

#include "sceneloaderbase.h"

/////////////////////////////////////////////////////////////////////

namespace loader {
  
/////////////////////////////////////////////////////////////////////

class assimp :
  public base
{
  public:
    assimp() = default;
    virtual ~assimp() = default;
//     assimp(assimp const& rhs);
//     assimp& operator=(assimp const& rhs);
//     bool operator==(assimp const& rhs) const;
    
  protected:
    
  private:
    

  // interface from base
  public:
    virtual scene::node* load ( std::string const& fname );
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace loader 

#endif // sceneloaderloaderassimp_h


