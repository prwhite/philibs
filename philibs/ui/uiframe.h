/////////////////////////////////////////////////////////////////////
//
//    class: frame
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuiframe_h
#define uiuiframe_h

/////////////////////////////////////////////////////////////////////

#include "uibase.h"

/////////////////////////////////////////////////////////////////////

namespace ui {
  
/////////////////////////////////////////////////////////////////////

class frame :
  public base
{
  public:
    frame();
    virtual ~frame();
    //frame(frame const& rhs);
    //frame& operator=(frame const& rhs);
    //bool operator==(frame const& rhs) const;
    
    typedef std::vector< BaseRef > Bases;

      // The non-const methods automatically call setBoundsDirty.
      // Maybe that's overzealous for getChildren.
    void addChild ( base* pBase );
    void remChild ( base* pBase );
    Bases& getChildren () { return mChildren; }
    Bases const& getChildren () const { return mChildren; }

  protected:
    Bases mChildren;
    
  private:
    

  // interface from base
  public:

  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );

};

/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuiframe_h


