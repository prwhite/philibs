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
        typedef int32_t IntType;
        typedef float FloatType;

        binding () { resize (); }

        IntType* getInts () { return ( IntType* ) ( &mVals[ 0 ] ); }
        IntType const* getInts () const { return ( IntType const* ) ( &mVals[ 0 ] ); }
      
        FloatType* getFloats () { return ( FloatType * ) ( &mVals[ 0 ] ); }
        FloatType const* getFloats () const { return ( FloatType * ) ( &mVals[ 0 ] ); }

          /// Data type constants for bindings.
          /// @note Not currently doing unsigned types.
        enum Type { Float1, Float2, Float3, Float4, Int1, Int2, Int3, Int4, Matrix2, Matrix3, Matrix4, NumTypes };

          /// Indicates which staget the uniform belongs to.
          /// @note Only doing vert and frag right now.
        enum Stage { Vertex, Fragment };

          // TODO: This has too many params, switch to individual setters or pub data?
        void setNameStageTypeCount ( std::string const& name, Stage stage, Type type, size_t count = 1 ) { mName = name; mStage = stage; mType = type; mCount = count; resize (); }

        std::string const& getName () const { return mName; }
        Stage getStage () const { return mStage; }
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
                case Float1: case Int1: return 1;
                case Float2: case Int2: return 2;
                case Float3: case Int3: return 3;
                case Float4: case Int4: return 4;
                case Matrix2: return 4;
                case Matrix3: return 9;
                case Matrix4: return 16;
                default:
                {
                  PNIDBGSTR("case not handled, mtype out of range");
                  return 0;
                }
              }
            }

        typedef std::vector< IntType > Vals;

        std::string mName;
        Vals mVals;
        Stage mStage = Fragment;
        Type mType = Float4;
        size_t mCount = 1;
    };


      // Now on to uniform's public members

    uniform();
    virtual ~uniform();
//     uniform(uniform const& rhs);
//     uniform& operator=(uniform const& rhs);
//     bool operator==(uniform const& rhs) const;

    typedef std::unordered_map< std::string, binding > Bindings;
//    Bindings& getBindings () { return mVals; }
    Bindings const& getBindings () const { return mBindings; }

      /** Provides reference to bindings, and sets this object to "dirty"
          state.  Dirty state means any gl-cached objects should be regen'd
          and clearDirty should be called. Currently, there is no gl-side
          cache for uniforms, so this is just good practice */
    binding& uniformOp ( std::string const& which ) { setDirty (); return mBindings[ which ]; }

    void setDirty ( bool val = true ) const { mDirty = val; }
    void clearDirty () { mDirty = false; }
  
  protected:
  
  private:
    Bindings mBindings;
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


