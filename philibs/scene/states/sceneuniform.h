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
#include "scenedirty.h"

#include "pnidbg.h"

/////////////////////////////////////////////////////////////////////

namespace scene {
  

/////////////////////////////////////////////////////////////////////

class uniform :
  public state,
  public dirtyProp<uniform, uniform>
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

        IntType* getInts () { return ( IntType* ) ( mVals.data() ); }
        IntType const* getInts () const { return ( IntType const* ) ( mVals.data() ); }
      
        FloatType* getFloats () { return ( FloatType * ) ( mVals.data() ); }
        FloatType const* getFloats () const { return ( FloatType * ) ( mVals.data () ); }

        template< class ValType >
        void setValue ( ValType const& val )
          {
            static_assert ( std::is_standard_layout<ValType>::value, "this type cannot be used with the trivial generic setValue method");

            ValType* ptr = ( ValType* ) getFloats();
            *ptr = val;
          }

          /// Data type constants for bindings.
          /// @note Not currently doing unsigned types.
        enum Type { Float1, Float2, Float3, Float4, Int1, Int2, Int3, Int4, Matrix2, Matrix3, Matrix4, NumTypes };

          /// Indicates which stage the uniform belongs to.
          /// @note Only doing vert and frag right now.
        enum Stage { Vertex, Fragment };

          /// Indicates what coordinate space a uniform should be transformed into
          /// before handoff to the graphics system.  E.g., 'None' means no transform
          /// at all, and 'ModelView' means to transform from node space back through model
          /// and view transforms, etc.
          /// @note This only affects vectors and matrices (maybe on matrices).
          /// @note This is experimental, and more importantly, not implemented currently!!!
        enum Xform { None, Model, ModelView, ModelViewProjection };

          // TODO: This has too many params, switch to individual setters or pub data?
        void set ( Stage stage, Type type, size_t count = 1 ) { mStage = stage; mType = type; mCount = count; resize (); }
        void set ( Stage stage, Type type, Xform xform, size_t count = 1 ) { mStage = stage; mType = type; mXform = xform; mCount = count; resize (); }
      
        Stage getStage () const { return mStage; }
        Type getType () const { return mType; }
        size_t getCount () const { return mCount; }
      
        size_t size () const { return mVals.size (); }
      
        size_t getStorageSize () const { return mVals.size () * sizeof ( IntType ); }
      
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

        Vals mVals;
        Stage mStage = Fragment;
        Type mType = Float4;
        Xform mXform = None;
        size_t mCount = 1;
    };

      // Now on to uniform's public members
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
    binding& bindingOp ( std::string const& which ) { setDirty (); return mBindings[ which ]; }
  
      /// Convenience method allows more concise setting of simple uniforms, e.g.,
      /// float, vec2, vec3, vec4, matrix4, int.  Other types probably won't do
      /// well with this method, including any types with a count > 1.
    template< class Type >
    void bindingOp ( std::string const& which, Type const& val )
      {
        static_assert ( std::is_standard_layout<Type>::value, "this type cannot be used with the trivial generic bindingOp method");
      
        auto found = mBindings.find ( which );
        if ( found != mBindings.end () )
        {
          found->second.setValue(val);
        }
        else
        {
          PNIDBGSTR1T("failed to find uniform binding for " << which );
        }
      }
  
    binding& getBinding ( std::string const& which ) { return mBindings[ which ]; }

  protected:
  
  private:
    Bindings mBindings;
  

  // interface from state
  public:
    virtual uniform* dup () const { return new uniform ( *this ); }
    virtual void accept ( stateDd* dd ) const { dd->dispatch ( this ); }
    
  protected:
    

};

/////////////////////////////////////////////////////////////////////

} // end of namespace scene 

#endif // scenesceneuniform_h


