/////////////////////////////////////////////////////////////////////
//
//    class: group
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimgroup_h
#define animanimgroup_h

/////////////////////////////////////////////////////////////////////

#include "animbase.h"
#include "pniautoref.h"
#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////

namespace anim {
  
/////////////////////////////////////////////////////////////////////

class group :
  public base
{
  public:
    group();
//     virtual ~group();
//     group(group const& rhs);
//     group& operator=(group const& rhs);
//     bool operator==(group const& rhs) const;
    
      // Name, for debugging purposes mostly.
    void setName ( std::string const& name ) { mName = name; }
    std::string const& getName () const { return mName; }

      // Bases... the children of this animation group.
    typedef pni::pstd::autoRef< base > BaseRef;
    typedef std::vector< BaseRef > Bases;

    void addChild ( base* pBase ) 
        { 
          mBases.push_back ( pBase );
          pBase->mParent = this;
          setDirty ();
        }

    Bases& getBases () { return mBases; }
    Bases const& getBases () const { return mBases; }
    
  protected:
    virtual void updateBounds () const;
    virtual void setDirty ( bool val = true ) const { mDirty = val; }
    virtual void internalUpdate ( manager* pManager, TimeType tval );
    
  private:
    Bases mBases;
    std::string mName;
    mutable bool mDirty;
    TimeType mLastTval;


  protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimgroup_h


