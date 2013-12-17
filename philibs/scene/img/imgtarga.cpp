////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#include "imgtarga.h"
#include <cstring>
#include <cassert>
//#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

////////////////////////////////////////////////////////////////////

namespace img {

/////////////////////////////////////////////////////////////////////

targa::targa ( BufferType* buffer ) :
		mBuffer ( 0 ),
		mImgData ( 0 ),
		mAlphaBits ( 0 ),
		mType ( Unknown ),
		mError ( Uninitialized )
{
	// Sizeof is not right even with pragma pack because there is 
	// padding at the end.  Thus we test for the diff between the
	// first and last member which should be 18 - 1.
	assert ( &mHeader.imagedescriptor - &mHeader.idlength == RealHeaderLength - 1 );
	
	memset ( &mHeader, 0, sizeof ( header ) );
	
	mError = load ( buffer );
}

targa::~targa ()
{
	unload ();
}

targa::Error targa::load ( BufferType* buffer )
{
	// We don't clear the buffer members though.
	if ( ! buffer )
			return NullBuffer;
	
	// Copy out to avoid bus probs/errors.
	memcpy ( &mHeader, buffer, RealHeaderLength );
	
	// Validate that we support this type of image.
	// TODO: Improve error handling/feedback/options.
	
	if ( mHeader.idlength ) return IdNotSupported;
	if ( mHeader.datatypecode != 2 ) return DataTypeNotSupported;
	switch ( mHeader.bitsperpixel )
	{
		case 16:
			return BitsPerPixelNotSupported;	// Not enabled right now.
		case 24:
			mType = BGR888;
			break;
		case 32:
			mType = BGRA8888;
			break;
		default:
				return BitsPerPixelNotSupported;
	}

	mBuffer = buffer;
	mImgData = mBuffer + RealHeaderLength;
	mAlphaBits = mHeader.imagedescriptor & 0x0f;
	
	return NoError;
}

void targa::unload ()
{
	mBuffer = 0;
	mImgData = 0;
	mAlphaBits = 0;
	mType = Unknown;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

targa* targa::loadHelper ( const std::string& fname )
{
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
			
			targa* pImg = new targa ( 0 );
			
			if ( Error error = pImg->load ( buff ) )
			{
					printf ( "targa::loadHelper: Error loading image: %d\n", error );
					delete[] buff;
					delete pImg;
					pImg = 0;
			}
			
			fclose ( fid );
			
			return pImg;
		}
	}	
	
	return 0;
}

void targa::unloadHelper ( targa* pImg )
{
	assert ( pImg );
	
	if ( BufferType* pBuffer = pImg->getBuffer () )
	{
		delete[] pBuffer;
	}
	
	delete pImg;
}

/////////////////////////////////////////////////////////////////////

} // end namespace img

/////////////////////////////////////////////////////////////////////

