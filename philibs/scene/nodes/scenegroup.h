/////////////////////////////////////////////////////////////////////
//
//    class: group
//
/////////////////////////////////////////////////////////////////////

#ifndef scenescenegroup_h
#define scenescenegroup_h

/////////////////////////////////////////////////////////////////////

#include "scenetypes.h"
#include "scenenode.h"
#include "scenegraphdd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  
/////////////////////////////////////////////////////////////////////

class group :
  public node
{
  public:
     group();
//      virtual ~group();
//     group(group const& rhs);
//      group& operator=(group const& rhs);
//      bool operator==(group const& rhs) const;
    
        virtual void accept ( graphDd* pDd ) const { pDd->dispatch ( this ); }
        virtual node* dup () const { return new group ( *this ); }

  protected:
    
  private:
    void ool ();

  // interface from node
  public:
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenescenegroup_h


