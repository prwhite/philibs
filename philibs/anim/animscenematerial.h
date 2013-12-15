/////////////////////////////////////////////////////////////////////
//
//  class: sceneMaterial
//
/////////////////////////////////////////////////////////////////////

#ifndef animanimscenematerial_h
#define animanimscenematerial_h

/////////////////////////////////////////////////////////////////////

#include "animbase.h"

#include "pniautoref.h"
//#include "pnibox.h"
#include "pnivec4.h"

#include "scenematerial.h"
#include "sceneblend.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace anim {
    
/////////////////////////////////////////////////////////////////////

class sceneMaterial :
  public anim::base
{
  public:
    sceneMaterial ();
    //virtual ~sceneMaterial ();
    //sceneMaterial ( sceneMaterial const& rhs );
    //sceneMaterial& operator= ( sceneMaterial const& rhs );
    //bool operator== ( sceneMaterial const& rhs ) const;
    
    typedef pni::pstd::autoRef< scene::material > MatRef;
    
    void setMaterial ( scene::material* pMat ) { mMat = pMat; }
    scene::material* getMaterial () const { return mMat.get (); }
        
      // Set which property is being animated.
    enum Property
    {
      Diffuse,    // Default.
      Ambient,
      Specular,
      Emissive,
      Shininess // Will use key[ 0 ] only.
    };
    
    void setProperty ( Property prop ) { mProp = prop; }
    Property getProperty () const { return mProp; }
    
      // Manage keys.
    class key : 
      public pni::math::vec4
        {
          public:
            key& operator = ( pni::math::vec4 const& rhs )
                {
                  pni::math::vec4* const tmp = this;
                  *tmp = rhs;
                  return *this;
                }
                
            bool operator < ( key const& rhs ) const
                { return mTimeStamp < rhs.mTimeStamp; }
                
            bool operator < ( float val ) const
                { return mTimeStamp < val; }
          
            anim::TimeType mTimeStamp;
            // TODO: TCB params, Ease in/out.
        };
    typedef std::vector< key > Keys;
    
    Keys& keyOp () { setDirty (); return mKeys; }
    Keys& getKeys () { return mKeys; }
    Keys const& getKeys () const { return mKeys; }
    
    void initStartFromCur ();
    
  protected:
    Keys mKeys;
    MatRef mMat;
    Property mProp;

    void reSortKeys ();
    void applyValue ( pni::math::vec4 const& val );
    
  private:


    // interface from anim::base
  public:

  protected:
		
    virtual void updateBounds () const;
    virtual void internalUpdate ( manager* pManager, TimeType tval );

    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
};

/////////////////////////////////////////////////////////////////////

class simpleSceneFadeBase :
  public sceneMaterial
{
  public:
    simpleSceneFadeBase () :
      Threshold ( 0.001f )
        {
          setProperty ( Diffuse );
        }

    void initNode ( scene::node* pNode ) 
        { 
          mOrigMat = static_cast< scene::material* > (
              pNode->getState ( scene::state::Material ) );
              
          if ( mOrigMat )
            mMat = static_cast< scene::material* > ( mOrigMat->dup () );
          else 
            mMat = new scene::material;

          mMat->setOverride ( true );
          setMaterial ( mMat.get () );

          mOrigBlend = static_cast< scene::blend* > (
              pNode->getState ( scene::state::Blend ) );

          mNode = pNode;
                    
          mNode->setState ( mMat.get (), scene::state::Material );
          mNode->setState ( new scene::blend, scene::state::Blend );
          
          setXform ( anim::xform ( 0.0f, 1.0f ) );
        }
    scene::node* getNode () const { return mNode.get (); }

  protected:
    typedef pni::pstd::autoRef< scene::material > MatRef;
    typedef pni::pstd::autoRef< scene::node > NodeRef;
    typedef pni::pstd::autoRef< scene::blend > BlendRef;
    
    MatRef mOrigMat;
    MatRef mMat;
    NodeRef mNode;
    BlendRef mOrigBlend;
    float const Threshold;
    
    void collectRefs ( Refs& refs )
        {
          refs.push_back ( mOrigMat.get () );
          refs.push_back ( mMat.get () );
          refs.push_back ( mNode.get () );
        }
};

/////////////////////////////////////////////////////////////////////

class simpleSceneFadeOut :
  public simpleSceneFadeBase
{
  public:

    simpleSceneFadeOut ( scene::node* pNode, anim::TimeType duration )
        {
          initNode ( pNode );
          setDuration ( duration );
        }

    void setDuration ( float val )
        {
          keyOp ().resize ( 2 );
          mKeys[ 0 ] = mOrigMat->getDiffuse ();
          mKeys[ 0 ].mTimeStamp = 0.0f;
          
          mKeys[ 1 ] = mKeys[ 0 ];
          mKeys[ 1 ][ 3 ] = 0.0f;
          mKeys[ 1 ].mTimeStamp = val;
        }

  protected:
    
    virtual void internalUpdate ( manager* pManager, TimeType tval )
        {
          simpleSceneFadeBase::internalUpdate ( pManager, tval );
          
            // Done.
          if ( pni::math::Trait::equal ( tval, 1.0f, Threshold ) )
          {
            mNode->setTravMask ( scene::Draw, 0x00 );
            mNode->setState ( mOrigMat.get (), scene::state::Material );
            mNode->setState ( mOrigBlend.get (), scene::state::Blend );
          }
        }
};

/////////////////////////////////////////////////////////////////////

class simpleSceneFadeIn :
  public simpleSceneFadeBase
{
  public:

    simpleSceneFadeIn ( scene::node* pNode, anim::TimeType duration )
        {
          initNode ( pNode );
          setDuration ( duration );
        }

    void setDuration ( float val )
        {
          keyOp ().resize ( 2 );
          mKeys[ 0 ] = mOrigMat->getDiffuse ();
          mKeys[ 0 ][ 3 ] = 0.0f;
          mKeys[ 0 ].mTimeStamp = 0.0f;
          
          mKeys[ 1 ] = mKeys[ 0 ];
          mKeys[ 1 ].mTimeStamp = val;
        }

  protected:
    
    virtual void internalUpdate ( manager* pManager, TimeType tval )
        {
          simpleSceneFadeBase::internalUpdate ( pManager, tval );
          
            // Done.
          if ( pni::math::Trait::equal ( tval, 0.0f, Threshold ) )
          {
            mNode->setTravMask ( scene::Draw, ~0x00000000 );
          }

          if ( pni::math::Trait::equal ( tval, 1.0f, Threshold ) )
          {
            mNode->setState ( mOrigMat.get (), scene::state::Material );
            mNode->setState ( mOrigBlend.get (), scene::state::Blend );
          }
        }
};

/////////////////////////////////////////////////////////////////////

} // end of namespace anim 

#endif // animanimscenematerial_h


