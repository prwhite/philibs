/******************************************************************************

 @File         PVRTContext.h

 @Title        

 @Copyright    Copyright (C) 2004 - 2007 by Imagination Technologies Limited.

 @Platform     ANSI compatible

 @Description  Context specific stuff - i.e. 3D API-related.

******************************************************************************/
#ifndef _PVRTCONTEXT_H_
#define _PVRTCONTEXT_H_

#if defined(WIN32) && !defined(UNDER_CE)	/* Windows desktop */
#define _CRTDBG_MAP_ALLOC
#include <windows.h>
#endif

#include <stdio.h>
#include <CoreSurface/CoreSurface.h>
#include <OpenGLES/egl.h>
#include <OpenGLES/gl.h>

#ifndef min
 #define min(x,y) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef max
 #define max(x,y) ( ((x) > (y)) ? (x) : (y) )
#endif

/****************************************************************************
** Macros
****************************************************************************/
#define FREE(X)		{ if(X) { free(X); (X) = 0; } }
#define PVRTRGBA(r, g, b, a)   ((GLuint) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))

/****************************************************************************
** Defines
****************************************************************************/

#ifdef UNDER_CE					/* Pocket PC and WinCE 5.0 only */
#ifdef DEBUG
#define _ASSERT(X) { (X) ? 0 : DebugBreak(); }
#define _ASSERTE _ASSERT
#else
#define _ASSERT(X) /* */
#define _ASSERTE(X) /* */
#endif
#define _RPT0(a,b)
#define _RPT1(a,b,c)
#define _RPT2(a,b,c,d)
#define _RPT3(a,b,c,d,e)
#define _RPT4(a,b,c,d,e,f)
#else
#ifdef WIN32						/* Windows */
#include <tchar.h>
#include <crtdbg.h>
#else							/* Other OSes */
#ifdef DEBUG
#include <assert.h>
#define _CRT_WARN 0
#define _RPT0(a,b) printf(b)
#define _RPT1(a,b,c) printf(b, c)
#define _RPT2(a,b,c,d) printf(b,c,d)
#define _RPT3(a,b,c,d,e) printf(b,c,d,e)
#define _RPT4(a,b,c,d,e,f) printf(b,c,d,f)
#ifndef _ASSERT
#define _ASSERT(X) assert(X)
#endif
#define _ASSERTE(X) _ASSERT(X)
#else
#define _CRT_WARN 0
#define _RPT0(a,b)
#define _RPT1(a,b,c)
#define _RPT2(a,b,c,d)
#define _RPT3(a,b,c,d,e)
#define _RPT4(a,b,c,d,e,f)
#ifdef _ASSERT
#undef _ASSERT
#endif
#define _ASSERT(X) /* */
#define _ASSERTE(X) /* */
#endif
#endif
#endif

/****************************************************************************
** Enumerations
****************************************************************************/

/****************************************************************************
** Structures
****************************************************************************/
struct SPVRTContext
{
	int reserved;	// No context info for OGL.
};

/****************************************************************************
** Functions
****************************************************************************/


#endif /* _PVRTCONTEXT_H_ */

/*****************************************************************************
 End of file (PVRTContext.h)
*****************************************************************************/
