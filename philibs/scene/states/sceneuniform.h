/////////////////////////////////////////////////////////////////////
//
//    class: uniform
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneuniform_h
#define scenesceneuniform_h

/////////////////////////////////////////////////////////////////////

#include "scenestate.h"
#include "scenestatedd.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  

/////////////////////////////////////////////////////////////////////

class uniform :
  public state
{
  public:


      // This class is a massive hack.  All uniform values are stored
      // in an int vector... but, that is possibly cast to a float array
      // when accessed by getFloats.  This only works because the size of
      // float and uint32_t are the same, their zero representation is
      // identical, and the vector doesn't otherwize care about the contents.

    class binding final
    {
      public:
        binding () { resize (); }

        uint32_t* getInts () { return ( uint32_t* ) ( &mVals[ 0 ] ); }
        uint32_t const* getInts () const { return ( uint32_t const* ) ( &mVals[ 0 ] ); }
      
        float* getFloats () { return ( float * ) ( &mVals[ 0 ] ); }
        float const* getFloats () const { return ( float * ) ( &mVals[ 0 ] ); }

        enum Type { Float, Int, Matrix2, Matrix3, Matrix4 };
      
        void setTypeCount ( Type type, size_t count = 1 ) { mType = type; mCount = count; resize (); }
        Type getType () const { return mType; }
        size_t getCount () const { return mCount; }
      
        size_t size () const { return mVals.size (); }
      
      private:
      
        void resize ()
            {
              mVals.resize ( calcTypeElements() * mCount );
            }

        size_t calcTypeElements () const
            {
              switch ( mType )
              {
                case Float: case Int: return 1;
                case Matrix2: return 4;
                case Matrix3: return 9;
                case Matrix4: return 16;
              }
            }

        typedef std::vector< uint32_t > Vals;
        Vals mVals;
        Type mType = Float;
        size_t mCount = 1;
    };


      // Now on to uniform's public members

    uniform();
    virtual ~uniform();
//     uniform(uniform const& rhs);
//     uniform& operator=(uniform const& rhs);
//     bool operator==(uniform const& rhs) const;

    binding& uniformOp ( std::string const& which ) { setDirty (); return mVals[ which ]; }

    typedef std::unordered_map< std::string, binding > Vals;
//    Vals& getVals () { return mVals; }
    Vals const& getVals () const { return mVals; }
  
    void setDirty ( bool val = true ) const { mDirty = val; }
  
  
  protected:
  
  private:
    Vals mVals;
    mutable bool mDirty;
  

  // interface from state
  public:
    virtual state* dup () const { return new uniform ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneuniform_h


