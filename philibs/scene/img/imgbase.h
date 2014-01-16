/////////////////////////////////////////////////////////////////////
//
//    class: base
//
/////////////////////////////////////////////////////////////////////

#ifndef imgimgbase_h
#define imgimgbase_h

/////////////////////////////////////////////////////////////////////

#include "pnirefcount.h"
#include "pniautoref.h"
#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////

namespace img {
  
/////////////////////////////////////////////////////////////////////

class base :
  public pni::pstd::refCount
{
  protected:
    virtual ~base();
  public:
    base();
//     base(base const& rhs);
//     base& operator=(base const& rhs);
//     bool operator==(base const& rhs) const;

    // Common types.
    typedef unsigned int Dim;
    
    enum Format
      {
        Uninit = -1,
        RGB888,
        RGBX8888,
        RGBA8888,
        RGB565,
        RGBA4444,
        RGBA5551,
        Gray8,
        Alpha8,
        GrayAlpha88,
        FormatCount
      };
      
    // Data storage details.
    typedef unsigned char BufferType;
    typedef std::vector< BufferType > BufferData;
    
    class buffer :
      public pni::pstd::refCount,
      public BufferData
        {
          protected:
            void collectRefs ( pni::pstd::refCount::Refs& refs ) {}
        };
        
    typedef std::vector< pni::pstd::autoRef< buffer > > Buffers;
    
    Buffers mBuffers; // Public data!  First is mip-map level 0, etc.
    
      // Get starting address of 'which' mipmap buffer.
    BufferType* operator[] ( unsigned int which ) const 
        { 
          buffer* pBuf = mBuffers[ which ].get ();
          return &( *pBuf )[ 0 ];
        }
    
    void uniquifyBuffers ();
        
    //virtual bool init ( void* mem ) = 0;
    
    void setSize ( Dim width, Dim height, Dim pitch )
        { mWidth = width; mHeight = height; mPitch = pitch; }
    void getSize ( Dim& width, Dim& height, Dim& pitch ) const
        { width = mWidth; height = mHeight; pitch = mPitch; }
    void setFormat ( Format format ) { mFormat = format; }
    Format getFormat () const { return mFormat; }
 
    void alloc ( bool mipMapsToo = false );
    void cloneFormatAndSize ( base const* pBase, bool reallocToo = true );
 
    void setName ( std::string const& name ) { mName = name; }
    std::string const& getName () const { return mName; }
 
    // TRICKY: setDirty is const for traversals that want to lazily clear
    // dirty state.
// 		enum Dirty { DirtyFalse, DirtyTrue, SubLoad };
		enum Dirty { DirtyFalse, DirtyTrue };
 		void setDirty ( Dirty dirtyIn = DirtyTrue ) const { mDirty = dirtyIn; }
		Dirty getDirty () const { return mDirty; }

    bool hasAlpha () const
        {
          switch ( getFormat () )
          {
            case RGBA8888:
            case RGBA4444:
            case RGBA5551:
            case Alpha8:
            case GrayAlpha88:
              return true;
            default:
              return false;
          }
        }
        
    static size_t calcFormatStride ( Format format )
        {
          switch ( format )
          {
            case RGB888: return 3;
            case RGBX8888: return 4;
            case RGBA8888: return 4;
            case RGB565: return  2;
            case RGBA4444: return 2;
            case RGBA5551: return 2;
            case Gray8: return 1;
            case Alpha8: return 1;
            case GrayAlpha88: return 2;
            case Uninit: 
            default:
              return 0;
          }
        }
        
    void writeRaw ( std::string const& fname );
    
    bool read ( std::string const& fname );
    bool write ( std::string const& fname );
    
  protected:
    std::string mName;
    mutable Dirty mDirty;
    Format mFormat;
    Dim mWidth;
    Dim mHeight;
    Dim mPitch;
    
  private:
    

  // interface from pni::pstd::refCount
  public:
    
  protected:
    virtual void onDoDbg ();
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs );
};

/////////////////////////////////////////////////////////////////////

} // end of namespace img 

/////////////////////////////////////////////////////////////////////

#endif // imgimgbase_h


