/////////////////////////////////////////////////////////////////////
//
//    file: imgpvr.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE

#include "imgpvr.h"
#include <cstring>
//#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

#import <UIKit/UIKit.h>

#include "scenetypes.h"

/////////////////////////////////////////////////////////////////////

// Nicked from here: https://developer.apple.com/library/ios/samplecode/PVRTextureLoader/Introduction/Intro.html#//apple_ref/doc/uid/DTS40008121-Intro-DontLinkElementID_2

#import <UIKit/UIKit.h>

@interface PVRTexture : NSObject
{
    NSMutableArray *_imageData;
    
    GLuint _name;
    uint32_t _width, _height;
    GLenum _internalFormat;
    BOOL _hasAlpha;
}

- (id)initWithContentsOfFile:(NSString *)path;
- (BOOL)unpackPVRData:(NSData *)data;

@property (readonly) NSMutableArray* imageData;
@property (readonly) GLuint name;
@property (readonly) uint32_t width;
@property (readonly) uint32_t height;
@property (readonly) GLenum internalFormat;
@property (readonly) BOOL hasAlpha;
 
@end

#define PVR_TEXTURE_FLAG_TYPE_MASK  0xff
 
static char gPVRTexIdentifier[4] = { 'P', 'V', 'R', '!' };
 
enum
{
    kPVRTextureFlagTypePVRTC_2 = 24,
    kPVRTextureFlagTypePVRTC_4
};
 
typedef struct _PVRTexHeader
{
    uint32_t headerLength;
    uint32_t height;
    uint32_t width;
    uint32_t numMipmaps;
    uint32_t flags;
    uint32_t dataLength;
    uint32_t bpp;
    uint32_t bitmaskRed;
    uint32_t bitmaskGreen;
    uint32_t bitmaskBlue;
    uint32_t bitmaskAlpha;
    uint32_t pvrTag;
    uint32_t numSurfs;
} PVRTexHeader;
 
 
@implementation PVRTexture

@synthesize imageData = _imageData;
@synthesize name = _name;
@synthesize width = _width;
@synthesize height = _height;
@synthesize internalFormat = _internalFormat;
@synthesize hasAlpha = _hasAlpha;
 
 
- (BOOL)unpackPVRData:(NSData *)data
{
    BOOL success = FALSE;
    PVRTexHeader *header = NULL;
    uint32_t flags, pvrTag;
    uint32_t dataLength = 0, dataOffset = 0, dataSize = 0;
    uint32_t blockSize = 0, widthBlocks = 0, heightBlocks = 0;
    uint32_t width = 0, height = 0, bpp = 4;
    uint8_t *bytes = NULL;
    uint32_t formatFlags;
    
    header = (PVRTexHeader *)[data bytes];
    
    pvrTag = CFSwapInt32LittleToHost(header->pvrTag);
 
    if (gPVRTexIdentifier[0] != ((pvrTag >>  0) & 0xff) ||
        gPVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
        gPVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
        gPVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff))
    {
        return FALSE;
    }
    
    flags = CFSwapInt32LittleToHost(header->flags);
    formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
    
    if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
    {
        [_imageData removeAllObjects];
        
        if (formatFlags == kPVRTextureFlagTypePVRTC_4)
            _internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        else if (formatFlags == kPVRTextureFlagTypePVRTC_2)
            _internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    
        _width = width = CFSwapInt32LittleToHost(header->width);
        _height = height = CFSwapInt32LittleToHost(header->height);
        
        if (CFSwapInt32LittleToHost(header->bitmaskAlpha))
            _hasAlpha = TRUE;
        else
            _hasAlpha = FALSE;
        
        dataLength = CFSwapInt32LittleToHost(header->dataLength);
        
        bytes = ((uint8_t *)[data bytes]) + sizeof(PVRTexHeader);
        
        // Calculate the data size for each texture level and respect the minimum number of blocks
        while (dataOffset < dataLength)
        {
            if (formatFlags == kPVRTextureFlagTypePVRTC_4)
            {
                blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                widthBlocks = width / 4;
                heightBlocks = height / 4;
                bpp = 4;
            }
            else
            {
                blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
                widthBlocks = width / 8;
                heightBlocks = height / 4;
                bpp = 2;
            }
            
            // Clamp to minimum number of blocks
            if (widthBlocks < 2)
                widthBlocks = 2;
            if (heightBlocks < 2)
                heightBlocks = 2;
 
            dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
            
            [_imageData addObject:[NSData dataWithBytes:bytes+dataOffset length:dataSize]];
            
            dataOffset += dataSize;
            
            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }
                  
        success = TRUE;
    }
    
    return success;
}

- (id)initWithContentsOfFile:(NSString *)path
{
    if (self = [super init])
    {
        NSData *data = [NSData dataWithContentsOfFile:path];
        
        _imageData = [[NSMutableArray alloc] initWithCapacity:10];
        
        _name = 0;
        _width = _height = 0;
        _internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        _hasAlpha = FALSE;
        
        if (!data || ![self unpackPVRData:data] )
            self = nil;
    }
    
    return self;
}

@end

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

namespace img {
  
/////////////////////////////////////////////////////////////////////

pvr::pvr()
{
  
}

pvr::~pvr()
{
  
}

// pvr::pvr(pvr const& rhs)
// {
//   
// }
// 
// pvr& pvr::operator=(pvr const& rhs)
// {
//   
//   return *this;
// }
// 
// bool pvr::operator==(pvr const& rhs) const
// {
//   
//   return false;
// }

img::base::Format PVRFormatToImgFormat ( PVRTexture* tex )
{
  if ( tex.hasAlpha )
  {
    if ( tex.internalFormat == kPVRTextureFlagTypePVRTC_2 )
      return img::base::RGBA_PVRTC_2BPPV1;
    else
      return img::base::RGBA_PVRTC_4BPPV1;
  }
  else
  {
    if ( tex.internalFormat == kPVRTextureFlagTypePVRTC_2 )
      return img::base::RGB_PVRTC_2BPPV1;
    else
      return img::base::RGB_PVRTC_4BPPV1;
  }
}

  // TODO: Optimize... this was just thrown together from example code.
  // This is not super optimal... it copies into a series of nsdata elements,
  // then copies into img::base::buffers.

base* pvr::loadHelper ( std::string const& fname )
{
  NSString* nsfname = [NSString stringWithUTF8String:fname.c_str()];

  if ( PVRTexture* tex = [[PVRTexture alloc] initWithContentsOfFile:nsfname] )
  {
    img::base* img = new img::base;
    img->setSize(tex.width, tex.height, [tex.imageData count]);
    img->setName(fname);
    img->setFormat( PVRFormatToImgFormat(tex));

    for ( NSData* data in tex.imageData)
    {
      size_t len = [data length];
      img::base::buffer* buff = new img::base::buffer;

      [data getBytes:&buff[ 0 ] length:len];

      img->mBuffers.push_back(buff);
    }

    return 0;
  }
  else
    return nullptr;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


} // end of namespace img 


