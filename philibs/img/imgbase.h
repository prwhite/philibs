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
    virtual ~base() = default;

  public:
    base() = default;
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
        RGB_PVRTC_2BPPV1, //GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
        RGB_PVRTC_4BPPV1, //GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
        RGBA_PVRTC_2BPPV1, //GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
        RGBA_PVRTC_4BPPV1, //GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
        DEPTH_COMPONENT16,
        DEPTH_COMPONENT24,
        DEPTH_COMPONENT32,
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
            void collectRefs ( pni::pstd::refCount::Refs& refs ) const {}
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
            case DEPTH_COMPONENT16: return 2;
            case DEPTH_COMPONENT24: return 3;
            case DEPTH_COMPONENT32: return 4;
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
    mutable Dirty mDirty = DirtyTrue;
    Format mFormat = Uninit;
    Dim mWidth = 0;
    Dim mHeight = 0;
    Dim mPitch = 0;
    
  private:
    

  // interface from pni::pstd::refCount
  public:
    
  protected:
    virtual void onDoDbg ();
    virtual void collectRefs ( pni::pstd::refCount::Refs& refs ) const;
};

/////////////////////////////////////////////////////////////////////

} // end of namespace img 

/////////////////////////////////////////////////////////////////////

#endif // imgimgbase_h


