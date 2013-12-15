/////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////


#include "imgdds.h"
#include <string>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
 

#define TOSTR(val) #val

/////////////////////////////////////////////////////////////////////

class ddsDbg :
  public img::dds
{
  protected:
    ~ddsDbg () {}

  public:
    void load ( std::string const& fname )
        {
          
        }
        
    void pd ( char* what, dword val )
        {
          printf ( "%s = %d\n", what, val );
        }
    
    void px ( char* what, dword val )
        {
          printf ( "%s = %0X\n", what, val );
        }
    
    
    void dbg ( BufferType* buff )
        {
          ddsHeader* header = reinterpret_cast< ddsHeader* > ( buff );

          pd ( TOSTR(header->dwMagic), header->dwMagic);

          pd ( TOSTR(header->surface.dwSize), header->surface.dwSize);
          px ( TOSTR(header->surface.dwFlags), header->surface.dwFlags);
          pd ( TOSTR(header->surface.dwHeight), header->surface.dwHeight);
          pd ( TOSTR(header->surface.dwWidth), header->surface.dwWidth);
          pd ( TOSTR(header->surface.dwPitchOrLinearSize), header->surface.dwPitchOrLinearSize);
          pd ( TOSTR(header->surface.dwDepth), header->surface.dwDepth);
          pd ( TOSTR(header->surface.dwMipMapCount), header->surface.dwMipMapCount);

          pd ( TOSTR(header->surface.format.dwSize), header->surface.format.dwSize);
          px ( TOSTR(header->surface.format.dwFlags), header->surface.format.dwFlags);
          px ( TOSTR(header->surface.format.dwFourCC), header->surface.format.dwFourCC);
          pd ( TOSTR(header->surface.format.dwRGBBitCount), header->surface.format.dwRGBBitCount);
          px ( TOSTR(header->surface.format.dwRBitMask), header->surface.format.dwRBitMask);
          px ( TOSTR(header->surface.format.dwGBitMask), header->surface.format.dwGBitMask);
          px ( TOSTR(header->surface.format.dwBBitMask), header->surface.format.dwBBitMask);
          px ( TOSTR(header->surface.format.dwRGBAlphaBitMask), header->surface.format.dwRGBAlphaBitMask);

          px ( TOSTR(header->surface.caps.dwCaps1), header->surface.caps.dwCaps1);
          px ( TOSTR(header->surface.caps.dwCaps2), header->surface.caps.dwCaps2);
        }
    
};


/////////////////////////////////////////////////////////////////////

int main ( int argc, char* argv[] )
{
  typedef char BufferType;
  
  if ( argc != 2 )
    exit ( -1 );

  std::string fname ( argv[ 1 ] );

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
			fread ( buff, fileStat.st_size, 1, fid );
			
			ddsDbg* pImg = new ddsDbg ();
      pImg->ref ();
			
			if ( ! pImg->init ( buff ) )
			{
          pImg->dbg ( buff );
					printf ( "dds::loadHelper: Error loading image\n" );
					pImg->unref ();
					pImg = 0;
			}
      else
      {

        // Do some work with the buff here then save it back out.

        pImg->dbg ( buff );

			  delete[] buff;

			  fclose ( fid );

			  pImg->unref ();
      }
		}
	}
  
  img::base* test = img::dds::loadHelper ( argv[ 1 ] );  

  return 0;
}


