/////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////

#ifndef scenesceneogl_h
#define scenesceneogl_h

/////////////////////////////////////////////////////////////////////

#include <OpenGLES/ES2/gl.h>

//#define CheckGLError checkGlError(__FILE__,__LINE__);
//void checkGlError ( char const* file, int line )
//{
//	if ( GLint err = glGetError () )
//		printf ( "gl error = 0x%x at %s:%d\n", err, file, line );
//}

//#define DOCHECKGLERROR

#if defined DOCHECKGLERROR && defined DEBUG

  #define CheckGLError { checkGlError ( __FILE__, __LINE__ ); }

#else

  #define CheckGLError

#endif

namespace scene {

  inline void checkGlError ( char const* file, int line )
    {
      if ( GLint err = glGetError () )
        printf ( "gl error = 0x%x at %s:%d\n", err, file, line );
    }
}

/////////////////////////////////////////////////////////////////////

namespace scene {


} // end of namespace scene 

#endif // scenesceneogl_h


