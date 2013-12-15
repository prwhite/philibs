/////////////////////////////////////////////////////////////////////
//
//    class: uiBaseTrans
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimuibasetrans_h
#define animanimuibasetrans_h

/////////////////////////////////////////////////////////////////////

#include "animtrans.h"
#include "animcommon.h"

#include "pniautoref.h"

#include "pnivec3.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

class uiBaseTrans :
    public trans, 
    public matrixHelperUiBase
{
    public:
//         uiBaseTrans();
//         virtual ~uiBaseTrans();
//         uiBaseTrans(uiBaseTrans const& rhs);
//         uiBaseTrans& operator=(uiBaseTrans const& rhs);
//         bool operator==(uiBaseTrans const& rhs) const;
        


    protected:
        
    private:
        

	  // interface from anim::base
	public:
		
	protected:		
    virtual void finalize ()
        {
          mUiBase->matrixOp () = this->mHelper->updateMatrix ();
        }

	// interface from anim::matrixHelperUi
	public:
    virtual scene::matrix4& matrixOp () { return matrixHelperUiBase::matrixOp (); }
    virtual scene::matrix4 const& getMatrix () const { return matrixHelperUiBase::getMatrix (); }
		
	protected:
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );		

};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimuibasetrans_h


