/////////////////////////////////////////////////////////////////////
//
//  class: particles
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneparticles_h
#define scenesceneparticles_h

/////////////////////////////////////////////////////////////////////

#include "scenesprite.h"

#include "pnirefcount.h"

#include <vector>

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

class particles :
  public sprites
{
  public:
    particles ();
    //virtual ~particles ();
    //particles ( particles const& rhs );
    //particles& operator= ( particles const& rhs );
    //bool operator== ( particles const& rhs ) const;

    typedef unsigned int BitsType;

    enum ParticleBindings
        {
          Vel = NextSpriteBinding,
          Age = Vel + 1,
          Bits = Age + 1
        };

      //////////////
      // Particle actions
    struct action :
      public pni::pstd::refCount
    {
      typedef scene::spriteData::iterator Iter;
    
      virtual void update ( 
          graphDd::fxUpdate const& update, particles* pParts ) = 0;
          
      protected:
        virtual void collectRefs ( pni::pstd::refCount::Refs& refs )
        {
          
        }
    };

    typedef pni::pstd::autoRef< action > ActionRef;
    typedef std::vector< ActionRef > Actions;
    
    Actions& getActions () { return mActions; }
    Actions const& getActions () const { return mActions; }
    
      //////////////
      // Particle state
    struct particle :
      public sprite
    {
      protected:
        particle () {}  // Can't instantiate this.

      public:
        pni::math::vec3 mVel;
        float mAge;
        BitsType mBits;
    };

  protected:
    void updateParticles ( graphDd::fxUpdate const& update );
    
    Actions mActions;
    TimeType mLastUpdate;
    
  private:


    // interface from sprite
  public:

    virtual void update ( graphDd::fxUpdate const& update );

  protected:
		virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneparticles_h


