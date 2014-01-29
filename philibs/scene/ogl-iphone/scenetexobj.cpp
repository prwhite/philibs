/////////////////////////////////////////////////////////////////////
//
//    file: scenetexobj.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PNIPSTDLOGDISABLE

#include <OpenGLES/ES2/glext.h>

#include "pnilog.h"

#include "scenetexobj.h"

#include "sceneogl.h"

#include <iostream>

using namespace std;

/////////////////////////////////////////////////////////////////////

namespace scene {
    
/////////////////////////////////////////////////////////////////////

texObj::texObj():
  mId ( 0 )
{
  init ();
}

texObj::~texObj()
{
  clear ();
}

// texObj::texObj(texObj const& rhs)
// {
//   init ();
// }

// texObj& texObj::operator=(texObj const& rhs)
// {
//     
//     return *this;
// }
// 
// bool texObj::operator==(texObj const& rhs) const
// {
//     
//     return false;
// }

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

inline
int
imageFormatToGlComponents ( img::base::Format formatIn ) // img::image* imgIn )
{
	switch ( formatIn )
	{
		default:
		case img::base::RGB888: return 3;
		case img::base::RGBX8888: return 4;
		case img::base::RGBA8888: return 4;
    case img::base::RGB565: return 2;
    case img::base::RGBA4444: return 2;
    case img::base::RGBA5551: return 2;
// 		case img::base::B8G8R8: return 3;
// 		case img::base::B8G8R8A8: return 4;
		case img::base::Gray8: return 1;
		case img::base::Alpha8: return 1;
		case img::base::GrayAlpha88: return 2;
	}
}

inline
int
imageFormatToGlFormat ( img::base::Format formatIn )	// img::image* imgIn )
{
	switch ( formatIn )
	{
		default:
		case img::base::RGB888: return GL_RGB;
		case img::base::RGBX8888: return GL_RGBA;
		case img::base::RGBA8888: return GL_RGBA;
    case img::base::RGB565: return GL_RGB;
    case img::base::RGBA4444: return GL_RGBA;
    case img::base::RGBA5551: return GL_RGBA;
// 		case img::base::BGR: return GL_BGR_EXT;
// 		case img::base::BGRA: return GL_BGRA_EXT;
		case img::base::Gray8: return GL_LUMINANCE;
		case img::base::Alpha8: return GL_ALPHA;
		case img::base::GrayAlpha88: return GL_LUMINANCE_ALPHA;

	}
}

inline
int
imageFormatToGlType ( img::base::Format typeIn )	// img::image* imgIn )
{
  // TODO: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, and GL_UNSIGNED_SHORT_5_5_5_1

	switch ( typeIn )
	{
		default:
		case img::base::RGB888: return GL_UNSIGNED_BYTE;
		case img::base::RGBX8888: return GL_UNSIGNED_BYTE;
		case img::base::RGBA8888: return GL_UNSIGNED_BYTE;
    case img::base::RGB565: return GL_UNSIGNED_SHORT_5_6_5;
    case img::base::RGBA4444: return GL_UNSIGNED_SHORT_4_4_4_4;
    case img::base::RGBA5551: return GL_UNSIGNED_SHORT_5_5_5_1;
		case img::base::Gray8: return GL_UNSIGNED_BYTE;
		case img::base::Alpha8: return GL_UNSIGNED_BYTE;
		case img::base::GrayAlpha88: return GL_UNSIGNED_BYTE;
    case img::base::RGB_PVRTC_2BPPV1: return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
    case img::base::RGB_PVRTC_4BPPV1: return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
    case img::base::RGBA_PVRTC_2BPPV1: return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    case img::base::RGBA_PVRTC_4BPPV1: return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
	}
}

inline
int textureTargetToGlTarget ( texture::Target target )
{
  switch ( target )
  {
    case texture::Tex2DTarget: return GL_TEXTURE_2D;
    case texture::CubeMapTarget: return GL_TEXTURE_CUBE_MAP;
  }
}

inline
int imageIdToGlTargetParam ( texture::Target target, texture::ImageId id )
{
  if ( target == texture::Tex2DTarget )
  {
    return GL_TEXTURE_2D;
  }
  else  // Until we have another target type, the else is for cube maps.
  {
    switch ( id )
    {
      default:
      case texture::CubePosX: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
      case texture::CubeNegX: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
      case texture::CubePosY: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
      case texture::CubeNegY: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
      case texture::CubePosZ: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
      case texture::CubeNegZ: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    }
  }
}

void
setGlImage ( texture const* texIn, texture::ImageId imgId, img::base const* imgIn, int level = 0 )
{
	unsigned int targetGl = imageIdToGlTargetParam ( texIn->getTarget(), imgId );
// 	unsigned int components = imageFormatToGlComponents ( imgIn->getFormat () );
// 	unsigned int components = imageFormatToGlFormat ( imgIn->getFormat () );
  img::base::Dim width, height, pitch;
  imgIn->getSize ( width, height, pitch );
  
  width >>= level;
  height >>= level;

  if ( width < 1 ) width = 1;
  if ( height < 1 ) height = 1;

	unsigned int format = imageFormatToGlFormat ( imgIn->getFormat () );
	unsigned int type = imageFormatToGlType ( imgIn->getFormat () );

// printf ( "1 format =       %d\n", imgIn->getFormat () );
// printf ( "2 format, type = %x, %x\n", format, type );

// 	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );		// is this slow?
CheckGLError

	glTexImage2D (
		targetGl,
		level,
		format,
		width,
		height,
		0,			// border
		format,
		type,
		( *imgIn )[ level ]
	);
CheckGLError

// 	glPixelStorei ( GL_UNPACK_ALIGNMENT, 4 );	// this should restore the ogl default
CheckGLError
}

int minFilterToGl ( texture::MinFilter filter )
{
	switch ( filter )
	{
		default:
		case texture::MinNearest: return GL_NEAREST;
		case texture::MinLinear: return GL_LINEAR;
		case texture::MinNearestMipNearest: return GL_NEAREST_MIPMAP_NEAREST;
		case texture::MinLinearMipNearest: return GL_LINEAR_MIPMAP_NEAREST;
		case texture::MinNearestMipLinear: return GL_NEAREST_MIPMAP_LINEAR;
		case texture::MinLinearMipLinear: return GL_LINEAR_MIPMAP_LINEAR;
	}
}

int magFilterToGl ( texture::MagFilter filter )
{
	switch ( filter )
	{
		default:
		case texture::MagNearest: return GL_NEAREST;
		case texture::MagLinear: return GL_LINEAR;
	}
}

int wrapToGl ( texture::Wrap wrap )
{
	switch ( wrap )
	{
		default:
		case texture::Repeat: return GL_REPEAT;
 		case texture::ClampToEdge: return GL_CLAMP_TO_EDGE;
    case texture::RepeatMirrored: return GL_MIRRORED_REPEAT;
	}
}

float textureAnisotropyToGl ( float valIn )
{
  static bool first = true;
  static float glMax = 2.0f;

  if ( first )
  {
    glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glMax );

    glMax -= 1.0f;
    first = false;
  }

  valIn *= glMax;
  valIn += 1.0f;

  return valIn;
}

void setGlTexParams ( texture const* texIn, unsigned int textureTarget )
{
	glTexParameteri ( textureTarget, GL_TEXTURE_MIN_FILTER,
			minFilterToGl ( texIn->getMinFilter () ) );

	glTexParameteri ( textureTarget, GL_TEXTURE_MAG_FILTER,
			magFilterToGl ( texIn->getMagFilter () ) );

	glTexParameteri ( textureTarget, GL_TEXTURE_WRAP_S,
			wrapToGl ( texIn->getSWrap () ) );

	glTexParameteri ( textureTarget, GL_TEXTURE_WRAP_T,
			wrapToGl ( texIn->getTWrap () ) );

#ifdef GL_ES_VERSION_3_0
	glTexParameteri ( textureTarget, GL_TEXTURE_WRAP_R,
			wrapToGl ( texIn->getRWrap () ) );
#endif // GL_ES_VERSION_3_0

#ifdef GL_ES_VERSION_2_0
 	glTexParameterf ( textureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, textureAnisotropyToGl ( texIn->getMaxAnisotropy () ) );
#endif // GL_ES_VERSION_2_0
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void texObj::bind ( texture const* pTex )
{
  glBindTexture ( mGlTextureTarget, mId );
}

void texObj::configOneTextureImg ( texture const* pTex, texture::ImageId imgId, img::base const* pImg )
{
PNIPSTDLOG
  if ( pTex->getMinFilter () > texture::MinLinear )
  {
//printf ( "image %s is mip-mapped\n", pTex->getImage ()->getName ().c_str () );

    // Iterate through buffers in img calling setGlImage.
    
    for ( unsigned int num = 0;
        num < pImg->mBuffers.size ();
        ++num )
    {
      setGlImage ( pTex, imgId, pImg, num );
CheckGLError
    }
  }
  else
  {
//printf ( "image %s is not mip-mapped\n", pTex->getImage ()->getName ().c_str () );
    // Do setGlImage for level 0.
    setGlImage ( pTex, imgId, pImg, 0 );
CheckGLError
  }
}


void texObj::config ( texture const* pTex )
{
  mGlTextureTarget = textureTargetToGlTarget ( pTex->getTarget () );
  
CheckGLError
  bind ( pTex );
CheckGLError

  bool hasMipMaps = true;

  size_t end = pTex->getNumImages ();
  for ( size_t num = 0; num < end; ++num )
  {
    texture::ImageId imgId = static_cast< texture::ImageId > ( num );
    if ( img::base* pImg = pTex->getImage ( imgId ) )
    {
      if ( pImg->getDirty () )
      {
PNIPSTDLOG
        configOneTextureImg ( pTex, imgId, pImg );
        pImg->setDirty ( img::base::DirtyFalse );
        if ( pImg->mBuffers.size () == 1 )
          hasMipMaps = false;
      }
    }
  }
CheckGLError

  if ( pTex->getDirty () == texture::DirtyTrue )
  {
    setGlTexParams ( pTex, mGlTextureTarget );

    // Generate mipmaps if texture is set to mipmap but only one image buffer is present.
    // Note, the current file loader doesn't do this, so it will take action by the app to
    // get this to be invoked.
    glHint ( GL_GENERATE_MIPMAP_HINT, GL_FASTEST ); // Don't need to call this this much. :(
    if ( pTex->getMinFilter () > texture::MinLinear && ! hasMipMaps )
      glGenerateMipmap ( mGlTextureTarget );
CheckGLError

    pTex->setDirty ( texture::DirtyFalse );
  }
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void texObj::init ()
{
  glGenTextures ( 1, &mId );
}

void texObj::clear ()
{
  glDeleteTextures ( 1, &mId );
}

/////////////////////////////////////////////////////////////////////
// overrides from attr


/////////////////////////////////////////////////////////////////////


} // end of namespace scene 


