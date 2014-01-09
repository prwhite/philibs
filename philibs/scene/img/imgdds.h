/////////////////////////////////////////////////////////////////////
//
//    class: dds
//
/////////////////////////////////////////////////////////////////////

#ifndef imgimgdds_h
#define imgimgdds_h

/////////////////////////////////////////////////////////////////////

#include "imgbase.h"
#include <string>

/////////////////////////////////////////////////////////////////////

namespace img {
  
/////////////////////////////////////////////////////////////////////

class dds
{
  protected:
    virtual ~dds();

  public:
  	typedef pni::pstd::autoRef< img::base > ImgRef;

    dds();
//     dds(dds const& rhs);
//     dds& operator=(dds const& rhs);
//     bool operator==(dds const& rhs) const;
    
    
    static base* loadHelper ( std::string const& fname );
    
  protected:
    typedef int dword;
  
    struct ddsHeader 
    {
      dword dwMagic;  // Must be "DDS "
      
      struct ddSurfaceDesc2
      {
        dword dwSize;
        dword dwFlags;
        dword dwHeight;
        dword dwWidth;
        dword dwPitchOrLinearSize;
        dword dwDepth;
        dword dwMipMapCount;
        dword dwReserved1[ 11 ];  // 18

        struct ddPixelFormat
        {
          dword dwSize;
          dword dwFlags;
          dword dwFourCC;
          dword dwRGBBitCount;
          dword dwRBitMask;
          dword dwGBitMask;
          dword dwBBitMask;
          dword dwRGBAlphaBitMask;  // 8
        } format;

        struct ddsCaps
        {
          dword dwCaps1;
          dword dwCaps2;
          dword reserved[ 2 ];  // 4
        } caps;

        dword dwReserved2;  // 1
      } surface;
    };  

    // The dwFlags member of the modified DDSURFACEDESC2 structure can be set to one or more of the following values.
    enum Flags
    {
      DDSD_CAPS	= 0x00000001,
      DDSD_HEIGHT	= 0x00000002,
      DDSD_WIDTH	= 0x00000004,
      DDSD_PITCH	= 0x00000008,
      DDSD_PIXELFORMAT	= 0x00001000,
      DDSD_MIPMAPCOUNT	= 0x00020000,
      DDSD_LINEARSIZE	= 0x00080000,
      DDSD_DEPTH	= 0x00800000,
      // pixel format flags
      DDPF_ALPHAPIXELS	= 0x00000001,
      DDPF_FOURCC	= 0x00000004,
      DDPF_RGB	= 0x00000040,
      DDPF_LUM  = 0x00020000,    // Reverse engineered from gimp dds output.
      DDS_UNCOMPRESSED = DDPF_ALPHAPIXELS | DDPF_RGB | DDPF_LUM
    };

    // The dwCaps1 member of the DDSCAPS2 structure can be set to one or more of the following values.
    enum caps1
    {
      DDSCAPS_COMPLEX	= 0x00000008,
      DDSCAPS_TEXTURE	= 0x00001000,
      DDSCAPS_MIPMAP	= 0x00400000
    };

    // The dwCaps2 member of the DDSCAPS2 structure can be set to one or more of the following values.
    enum caps2
    {
      DDSCAPS2_CUBEMAP	= 0x00000200,
      DDSCAPS2_CUBEMAP_POSITIVEX	= 0x00000400,
      DDSCAPS2_CUBEMAP_NEGATIVEX	= 0x00000800,
      DDSCAPS2_CUBEMAP_POSITIVEY	= 0x00001000,
      DDSCAPS2_CUBEMAP_NEGATIVEY	= 0x00002000,
      DDSCAPS2_CUBEMAP_POSITIVEZ	= 0x00004000,
      DDSCAPS2_CUBEMAP_NEGATIVEZ	= 0x00008000,
      DDSCAPS2_VOLUME	= 0x00200000
    };

    // Methods.
    bool determineFormat ();
    bool determineCaps ();
    base::buffer* createBuffer ( void* start, size_t length );
    void swapSrcBuffer ( char* mem, size_t size );

    // Data members.
    ddsHeader mHeader;
    base::Format mFormat;
    base* mImg;

    bool init ( void* mem );


};

/////////////////////////////////////////////////////////////////////

} // end of namespace img 

#endif // imgimgdds_h


