/////////////////////////////////////////////////////////////////////
//
//    class: actions
//
/////////////////////////////////////////////////////////////////////

#ifndef uiuiactions_h
#define uiuiactions_h

/////////////////////////////////////////////////////////////////////

#include "uibase.h"

#include "scenematerial.h"

/////////////////////////////////////////////////////////////////////

namespace ui {
  
/////////////////////////////////////////////////////////////////////

//class showHideAction :
//  public action
//{
//    virtual void init ( base* pBase )
//        {
//          pBase->getRootNode ()->addChild ( mRootNode.get () );
//        }
//
//    virtual void execute ( action* pLast )
//        {
//          mRootNode->setTravMask ( scene::Draw, ~0x00 );
//          mRootNode->setTravMask ( scene::Isect, ~0x00 );
//        }
//    virtual void finalize ( action* pLast )
//        {
//          mRootNode->setTravMask ( scene::Draw, 0x00 );
//          mRootNode->setTravMask ( scene::Isect, 0x00 );
//        }
//};

/////////////////////////////////////////////////////////////////////

class disabledAlphaAction :
  public action
{
    pni::math::vec4 mOrigColor;

  public:
    virtual void execute ( base* pBase )
        {
          using namespace scene;
          node* pNode = pBase->getBaseNode ();

          material* pMat = static_cast< material* > (
              pNode->getState ( state::Material ) );
          
          if ( pMat )
          {
            pNode->uniquifyState ( state::Material );
            pMat = static_cast< material* > (
              pNode->getState ( state::Material ) );
            mOrigColor = pMat->getDiffuse ();
            pMat->setOverride ( true );
          }
          else
          {
            pMat = new material;
            pMat->setOverride ( true );
          }

          pMat->setDiffuse ( pni::math::vec4 ( 1.0f, 1.0f, 1.0f, 0.25f ) );
        }

    virtual void finalize ( base* pBase )
        {
          using namespace scene;
          node* pNode = pBase->getBaseNode ();

          material* pMat = static_cast< material* > (
              pNode->getState ( state::Material ) );
          
          if ( pMat )
          {
            if ( pMat->getOverride () )
            {
              pNode->setState ( 0, state::Material );
            }
            else
            {
              pMat->setDiffuse ( mOrigColor );
            }
            
          }
          else
          {
            // Freak out!
          }
        }
            
};

  // Just a dummy to get other actions to execute their 'finalize'.
class noopAction :
  public action
{
  public:
            
};


/////////////////////////////////////////////////////////////////////

} // end of namespace ui 

#endif // uiuiactions_h


