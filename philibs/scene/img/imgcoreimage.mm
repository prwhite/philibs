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

      // Allocate a buffer for the incoming image data.
    base::buffer* dst = new base::buffer;
    dst->resize ( width * height * sizeof ( uint32_t ) );

      // Set up drawing context for image.
    CGContextRef spriteContext = CGBitmapContextCreate( &( *dst )[ 0 ], width, height, 8, width*4,
        CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);

      // Fill image into destination
    CGContextDrawImage(spriteContext, CGRectMake(0, 0, width, height), spriteImage);

    CGContextRelease(spriteContext);

      // Configure new scene image.
    base* img = new base;
    img->mBuffers.push_back(dst);
    img->setSize(width, height, width); // pitch = width because we're allocating uint32_t for each pixel.
    img->setFormat(base::RGBA8888);
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


