////////////////////////////////////////////////////////////////////
//
// Memory Management: the img::targa class does no memory
//   management.  The buffer pointer must have the same
//   lifetime as the class instance, or longer.
//
////////////////////////////////////////////////////////////////////

#ifndef targaimg_h
#define targaimg_h

////////////////////////////////////////////////////////////////////

#ifndef WIN32
  #include <cstdint>
#else
  typedef unsigned char uint8_t;
  typedef unsigned short uint16_t;
  typedef unsigned int uint32_t;
#endif
#include <string>

#include "imgbase.h"

/////////////////////////////////////////////////////////////////////

namespace img {

/////////////////////////////////////////////////////////////////////


class targa 
{
   typedef char BufferType;

   enum Type {
     BGR888,
     BGRA8888,
//     BGR5551,
     Unknown
   };

	public:
		enum Error {
			NoError = 0,
			Uninitialized,
			NullBuffer,
			IdNotSupported,
			DataTypeNotSupported,
			BitsPerPixelNotSupported
		};
		
		targa ( BufferType* buffer = 0 );
		~targa ();
		
		// Just sets/clers internal data members while aliasing
		// the buffer memory.
		Error load ( BufferType* buffer );
		void unload ();
		
		// Allocates a new img::targa object.  Don't forget to also
		// delete the buffer pointer or use unloadHelper.
		static targa* loadHelper ( const std::string& fname );
		static void unloadHelper ( targa* pImg );
		
		Type getType () const { return mType; }
		uint32_t getWidth () const { return mHeader.width; }
		uint32_t getHeight () const { return mHeader.height; }
		BufferType* getBuffer () const { return mBuffer; }
		BufferType* getImgData () const { return mImgData; }
		
	private:
		enum Info { RealHeaderLength = 18 };
		
		// Based on http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/
#pragma pack(push,1)
			struct header {
			uint8_t  idlength;
			uint8_t  colourmaptype;
			uint8_t  datatypecode;
			uint16_t colourmaporigin;
			uint16_t colourmaplength;
			uint8_t  colourmapdepth;
			uint16_t x_origin;
			uint16_t y_origin;
			uint16_t width;
			uint16_t height;
			uint8_t  bitsperpixel;
			uint8_t  imagedescriptor;
		};
#pragma pack(pop)

		header mHeader;
		BufferType* mBuffer;
		BufferType* mImgData;
		uint8_t mAlphaBits;
		Type mType;
		Error mError;
};


////////////////////////////////////////////////////////////////////

} // end namespace img

/////////////////////////////////////////////////////////////////////

#endif // targaimg_h
