/////////////////////////////////////////////////////////////////////
//
//    file: scenetexobj.cpp
//
/////////////////////////////////////////////////////////////////////

#define PNIDBGDISABLE
#define PNIPSTDLOGDISABLE

#include "pnilog.h"

#include "scenetexobj.h"

#include "sceneogl.h"

#include <iostream>

using namespace std;

/////////////////////////////////////////////////////////////////////

#define CheckGLError checkGlError2(__FILE__,__LINE__);
void checkGlError2 ( char const* file, int line )
{
	if ( GLint err = glGetError () )
		printf ( "gl error = 0x%x at %s:%d\n", err, file, line );
}

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

void texObj::bind ( texture const* pTex )
{
  glBindTexture ( GL_TEXTURE_2D, mId );
}

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
	}
}

void
setGlImage ( texture const* texIn, img::base* imgIn, int level = 0 )
{
	unsigned int targetGl = GL_TEXTURE_2D;
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

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


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
// 		case texture::Clamp: return GL_CLAMP;
// 		case texture::ClampToEdge: return GL_CLAMP_TO_EDGE;
	}
}

// float textureAnisotropyToGl ( float valIn )
// {
// 	static bool first = true;
// 	static float glMax = 2.0f;
// 
// 	if ( first )
// 	{
// 		glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glMax );
// 
// 		glMax -= 1.0f;
// 		first = false;
// 	}
// 
// 	valIn *= glMax;
// 	valIn += 1.0f;
// 
// 	return valIn;
// }

void setGlTexParams ( texture const* texIn )
{
// 	int targetGl = textureTargetToGlParam ( texIn->getTarget () );
	int targetGl = GL_TEXTURE_2D;

	glTexParameteri ( targetGl, GL_TEXTURE_MIN_FILTER, 
			minFilterToGl ( texIn->getMinFilter () ) );

	glTexParameteri ( targetGl, GL_TEXTURE_MAG_FILTER, 
			magFilterToGl ( texIn->getMagFilter () ) );

	glTexParameteri ( targetGl, GL_TEXTURE_WRAP_S,
			wrapToGl ( texIn->getSWrap () ) );

	glTexParameteri ( targetGl, GL_TEXTURE_WRAP_T,
			wrapToGl ( texIn->getTWrap () ) );

// 	glHint ( GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST );

// 	if ( extgl_Extensions.SGIS_generate_mipmap )
// 	{
// 		if ( texIn->getMinFilter () > texture::MinLinear )
// 			glTexParameteri ( targetGl, GL_GENERATE_MIPMAP_SGIS, 1 );
// 		else
// 			glTexParameteri ( targetGl, GL_GENERATE_MIPMAP_SGIS, 0 );
// 
// 	}

// 	glTexParameterf ( targetGl, GL_TEXTURE_MAX_ANISOTROPY_EXT, textureAnisotropyToGl ( texIn->getMaxAnisotropy () ) );
}

/////////////////////////////////////////////////////////////////////

void texObj::config ( texture const* pTex )
{
  img::base* pImg = pTex->getImage ();
  
  if ( ! pImg )
    return;
  
CheckGLError
  bind ( pTex );
CheckGLError
  
  if ( pTex->getDirty () == texture::DirtyTrue ||
      pImg->getDirty () != img::base::DirtyFalse )
  {
    if ( pTex->getDirty () == texture::DirtyTrue )
      setGlTexParams ( pTex );
PNIPSTDLOG
CheckGLError
    
    if ( pImg->getDirty () )
    {
PNIPSTDLOG
      if ( pTex->getMinFilter () > texture::MinLinear )
      {
//printf ( "image %s is mip-mapped\n", pTex->getImage ()->getName ().c_str () );

        // Iterate through buffers in img calling setGlImage.
        
        for ( unsigned int num = 0;
            num < pTex->getImage ()->mBuffers.size ();
            ++num )
        {
          setGlImage ( pTex, pTex->getImage (), num );
CheckGLError
        }
      }
      else
      {
//printf ( "image %s is not mip-mapped\n", pTex->getImage ()->getName ().c_str () );
        // Do setGlImage for level 0.
        setGlImage ( pTex, pTex->getImage (), 0 );
CheckGLError
      }
    }
    
    pTex->setDirty ( texture::DirtyFalse );
    pImg->setDirty ( img::base::DirtyFalse );
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


