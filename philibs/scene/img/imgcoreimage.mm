/////////////////////////////////////////////////////////////////////
//
//    file: imgcoreimage.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE

#include "imgcoreimage.h"
#include <cstring>
//#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

#import <UIKit/UIKit.h>

#include "scenetypes.h"
#include "pnidbg.h"

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

namespace img {
  
/////////////////////////////////////////////////////////////////////

coreImage::coreImage()
{
  
}

coreImage::~coreImage()
{
  
}

// coreImage::coreImage(coreImage const& rhs)
// {
//   
// }
// 
// coreImage& coreImage::operator=(coreImage const& rhs)
// {
//   
//   return *this;
// }
// 
// bool coreImage::operator==(coreImage const& rhs) const
// {
//   
//   return false;
// }


/////////////////////////////////////////////////////////////////////

  // This function kinda helps find the right img::base::Format for
  // the incoming image, but there are plenty of possible holes.
  // We'll refine this as we find gotchas.
base::Format imageInfoToBufferType ( CGImageRef cgi )
{
  size_t bpc = CGImageGetBitsPerComponent(cgi);
  size_t bpp = CGImageGetBitsPerPixel(cgi);
//  size_t Bpr = CGImageGetBytesPerRow(cgi);
  bool isMask = CGImageIsMask(cgi);
  CGImageAlphaInfo alphaInfo = CGImageGetAlphaInfo(cgi);
  bool hasAlpha = alphaInfo != kCGImageAlphaNone
      && alphaInfo != kCGImageAlphaNoneSkipFirst
      && alphaInfo != kCGImageAlphaNoneSkipLast;
  size_t components = alphaInfo == kCGImageAlphaNone ? 0 : 1;
  components += CGColorSpaceGetNumberOfComponents ( CGImageGetColorSpace(cgi) );

  base::Format ret = base::Uninit;

  if      ( components == 3 && bpc == 8 && ! hasAlpha && bpp == 24 ) ret = base::RGB888;
  else if ( components == 4 && bpc == 8 && ! hasAlpha && bpp == 32 ) ret = base::RGBX8888;
  else if ( components == 4 && bpc == 8 &&   hasAlpha && bpp == 32 ) ret = base::RGBA8888;
  else if ( components == 4 && bpc == 4 &&   hasAlpha && bpp == 16 ) ret = base::RGBA4444;
  else if ( components == 1 && bpc == 8 && ! hasAlpha && bpp ==  8 ) ret = base::Gray8;
  else if ( components == 1 && bpc == 8 &&   isMask   && bpp ==  8 ) ret = base::Alpha8;
  else if ( components == 2 && bpc == 8 &&   hasAlpha && bpp == 16 ) ret = base::GrayAlpha88;
  else
    PNIDBGSTR("image format not handled");

  return ret;
}

base* coreImage::loadHelper ( std::string const& fname )
{
    // Massaged from here: http://www.raywenderlich.com/4404/opengl-es-2-0-for-iphone-tutorial-part-2-textures

  NSString* fileName = [NSString stringWithUTF8String:fname.c_str()];

  CGImageRef spriteImage = [UIImage imageWithContentsOfFile:fileName].CGImage;
  if ( spriteImage )
  {
      // Get width/height
    size_t width = CGImageGetWidth(spriteImage);
    size_t height = CGImageGetHeight(spriteImage);
    size_t bpc = CGImageGetBitsPerComponent(spriteImage);
//    size_t bpp = CGImageGetBitsPerPixel(spriteImage);
    size_t Bpr = CGImageGetBytesPerRow(spriteImage);
    CGImageAlphaInfo alphaInfo = CGImageGetAlphaInfo(spriteImage);
//    size_t components = alphaInfo == kCGImageAlphaNone ? 3 : 4;

      // Allocate a buffer for the incoming image data.
    base::buffer* dst = new base::buffer;
    dst->resize ( Bpr * height );

      // Set up drawing context for image.
    CGContextRef spriteContext = CGBitmapContextCreate( &( *dst )[ 0 ], width, height, bpc, Bpr,
        CGImageGetColorSpace(spriteImage), alphaInfo);

      // Fill image into destination
    CGContextDrawImage(spriteContext, CGRectMake(0, 0, width, height), spriteImage);

    CGContextRelease(spriteContext);

      // Configure new scene image.
    base* img = new base;
    img->mBuffers.push_back(dst);
    img->setSize( ( img::base::Dim ) width, ( img::base::Dim ) height, ( img::base::Dim ) Bpr); // pitch = width because we're allocating uint32_t for each pixel.
    img->setFormat(imageInfoToBufferType(spriteImage));
    img->setName(fname);
    return img;
  }
  else
  {
    PNIDBGSTR("Failed to load image");
    PNIDBGSTR(fname);
    return 0;
  }
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


} // end of namespace img 


