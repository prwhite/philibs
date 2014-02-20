/////////////////////////////////////////////////////////////////////
//
//    file: imgdds.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE

#include "imgdds.h"
#include <cstring>
//#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

#include "scenetypes.h"
#include "pnidbg.h"

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

namespace img {
  
/////////////////////////////////////////////////////////////////////

dds::dds() :
  mFormat ( base::Uninit )
{
  
}

dds::~dds()
{
  
}

// dds::dds(dds const& rhs)
// {
//   
// }
// 
// dds& dds::operator=(dds const& rhs)
// {
//   
//   return *this;
// }
// 
// bool dds::operator==(dds const& rhs) const
// {
//   
//   return false;
// }


/////////////////////////////////////////////////////////////////////

base* dds::loadHelper ( std::string const& fname )
{
  dds ddsLoader;

  typedef char BufferType;

	FILE* fid = fopen ( fname.c_str (), "rb" );
	if ( fid )
	{
		struct stat fileStat;
		memset ( &fileStat, 0, sizeof ( fileStat ) );
		
		if ( fstat ( fileno ( fid ), &fileStat ) == 0 )
		{
//			printf ( "tga img data size = %d\n", fileStat.st_size - RealHeaderLength );
			
			BufferType* buff = new BufferType[ fileStat.st_size ];
			// TODO: Error check fread.
			fread ( buff, ( size_t ) fileStat.st_size, 1, fid );
			
			base* pImg = new base ();
      pImg->setName ( fname );
      ddsLoader.mImg = pImg;
			
			if ( ! ddsLoader.init ( buff ) )
			{
				printf ( "dds::loadHelper: Error loading image\n" );
				pImg->ref ();
				pImg->unref ();
				pImg = 0;
			}
			
			delete[] buff;
      
			fclose ( fid );
			
			return pImg;
		}
    else
      printf ( "dds::loading: could not find file %s\n", fname.c_str () );
    
	}	

	return 0;
}

/////////////////////////////////////////////////////////////////////
// overrides from base

bool dds::init ( void* mem )
{
  // Verify right kind of header.
  memcpy ( &mHeader, mem, sizeof( mHeader ) );

PNIDBG
  int magic;
  strncpy ( reinterpret_cast< char* > ( &magic ), "DDS ", 4 );
  if( mHeader.dwMagic != magic )
    return false;
    
PNIDBG
  if( mHeader.surface.dwSize != 124 )
    return false;
  
PNIDBG
  if ( ! determineFormat () )
    return false;
  
  mImg->setFormat ( mFormat );
  
PNIDBG
  if ( ! determineCaps () )
    return false;
  
  // Read in the mip map levels.
  base::Dim width = mHeader.surface.dwWidth;
  base::Dim height = mHeader.surface.dwHeight;
  base::Dim pitch = mHeader.surface.dwPitchOrLinearSize;
  
  mImg->setSize ( width, height, pitch );
  
PNIDBG
  if ( mHeader.surface.dwFlags & DDSD_LINEARSIZE )
    pitch = pitch / height;
  
  char* start = reinterpret_cast< char* > ( mem );
  start += sizeof ( mHeader ); // Best be 128!!!

PNIDBG
  if ( mHeader.surface.caps.dwCaps1 & DDSCAPS_MIPMAP )
  {
    for ( int num = 0; 
        num < mHeader.surface.dwMipMapCount;
        ++num )
    {
      // Really don't believe that this is totally nailed 
      // up, as mip maps might have more constraints on their
      // pitch.  We are assuming all textures are power-of-two.

      size_t size = pitch * height;

      createBuffer ( start, size );

      start += size;
      pitch /= 2;
      height /= 2;
      width /= 2;
      
      if ( height < 1 ) height = 1;
      if ( width < 1 ) width = 1;
    }
  }
  else
  {    
    createBuffer ( start, height * pitch );
  }
  
PNIDBG
  return true;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

bool dds::determineFormat ()
{
PNIDBG
  if ( ! ( mHeader.surface.format.dwFlags & DDS_UNCOMPRESSED ) )
    return false;
  
PNIDBG
  bool hasAlpha = mHeader.surface.format.dwFlags & DDPF_ALPHAPIXELS;
  size_t bits = mHeader.surface.format.dwRGBBitCount;
  const dword rMask = mHeader.surface.format.dwRBitMask;

  // TRICKY, BUG, TEMP: There are some issues being worked-around here.
  // The GIMP DDS exporter writes out a bogus format for BGR888, 
  // having 4 components, hence RGBX8888.  Also, of course, we're
  // obviously flipping endianness of these formats... so DDS BGR
  // becomes RGB for us.
  if ( hasAlpha )
  {
PNIDBG
//     if ( rMask == 0xff000000 )
//       mFormat = R8G8B8A8;
//     else if ( rMask == 0x000000ff )
//       mFormat = A8B8G8R8;
//     else
//       return false;
      if ( rMask == 0x000000ff )
        mFormat = base::RGBA8888;
      else if ( rMask == 0xf00 )
        mFormat = base::RGBA4444;
      else
        return false;
  }
  else if ( bits == 32 )
  {
PNIDBG
    // This is all wrong and just working around a bug with
    // the gimp dds exporter.
    if ( rMask == 0xff0000 )
      mFormat = base::RGBX8888;
    if ( rMask == 0xff )
      mFormat = base::RGBX8888;
    else  
      return false;
  }
  else if ( bits == 24 )
  {
PNIDBG
    if ( rMask == 0xff0000 )
      mFormat = base::RGB888;
//     else if ( rMask == 0x0000ff )
//       mFormat = BGR888;
//     else
//       return false;
    else if ( rMask == 0x0000ff )
      mFormat = base::RGB888;
    else
      return false;
  }
  else if ( bits == 16 )
  {
      // Always 565 because we catch other formats with alpha
      // in alpha case above.
    mFormat = base::RGB565;
  }
  else if ( bits == 8 )
  {
    if ( rMask )
      mFormat = base::Gray8;
    else if ( mHeader.surface.format.dwRGBAlphaBitMask )
      mFormat = base::Alpha8;
    else
      return false;
  }
  else
    return false;
  
  return true;
}

bool dds::determineCaps ()
{
  // dword caps1 = mHeader.surface.caps.dwCaps1;
  dword caps2 = mHeader.surface.caps.dwCaps2;
  
  // TEMP: Only supporting single-image textures and textures
  // with mip-maps right now.
  if ( ( caps2 & DDSCAPS2_CUBEMAP )
      || ( caps2 & DDSCAPS2_VOLUME ) )
    return false;
  
  
  return true;
}

base::buffer* dds::createBuffer ( void* start, size_t length )
{
  swapSrcBuffer ( ( char* ) start, length );

  base::buffer* buf = new base::buffer;
  buf->resize ( length );
  
  memcpy ( &buf->front (), start, length );
  
  mImg->mBuffers.push_back ( buf );

  return buf;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

struct rgba4444
{
  unsigned int r0 : 4;
  unsigned int g0 : 4;
  unsigned int b0 : 4;
  unsigned int a0 : 4;

  unsigned int r1 : 4;
  unsigned int g1 : 4;
  unsigned int b1 : 4;
  unsigned int a1 : 4;
  
  inline void swap ( const rgba4444 src )
      {
        r0 = src.a0;
        g0 = src.r0;
        b0 = src.g0;
        a0 = src.b0;

        r1 = src.a1;
        g1 = src.r1;
        b1 = src.g1;
        a1 = src.b1;
      }
};

inline unsigned int swap4444 ( unsigned char* srcPtr )
{
  unsigned int ret;
  rgba4444* dst = reinterpret_cast< rgba4444* > ( &ret );
  rgba4444* src = reinterpret_cast< rgba4444* > ( srcPtr );
  
  dst->swap ( *src );
  
  return ret;
}

/////////////////////////////////////////////////////////////////////

void dds::swapSrcBuffer ( char* mem, size_t size )
{
  switch ( mFormat )
  {
    case base::RGBA4444:
      {
        unsigned int* ptr = reinterpret_cast< unsigned int* > ( mem );
        size_t intSize = size / 4;
        
        for ( size_t num = 0; num < intSize; ++num )
        {
          *ptr = swap4444 ( reinterpret_cast< unsigned char* > ( ptr ) );
          ++ptr;
        }
      }
      break;
    // TODO handle other formats if necessary.
    default:
			printf ( "dds::loadHelper: unsupported format in swapSrcBuffer\n" );
      break;
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


} // end of namespace img 


