/////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////

#ifndef pniplatform_h
#define pniplatform_h

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

// NOTE: Some of these items are non-exclisive... e.g., iphone is 
// both unix and iphone target.


#ifdef WIN32

  #define PNI_TARGET_WIN32

#elif defined(__APPLE__)
  #include <TargetConditionals.h>

  #define PNI_TARGET_UNIX

  #ifdef TARGET_OS_IPHONE
  
    #define PNI_TARGET_IPHONE
    #define PNI_TARGET_IPHONE_SDK
    #define PNI_TARGET_IPHONE_DEVICE
    
  #elif defined(TARGET_IPHONE_SIMULATOR)
    
    #define PNI_TARGET_IPHONE
    #define PNI_TARGET_IPHONE_SDK
    #define PNI_TARGET_IPHONE_SIM
    
  #else

     // HACK for 1.1.x JB toolchain
    #if __APPLE_CC__ == 5449
      
      #define PNI_TARGET_IPHONE
      #define PNI_TARGET_IPHONE_JAILBREAK
    
    #else
  
      #define PNI_TARGET_OSX
    
    #endif
    
  #endif

#else // Linux/Unix
  
  #define PNI_TARGET_UNIX
  
  // TODO: Also figure out Linux/BSD Distinction.
  
#endif 

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {

/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////

	} // end of namespace pstd
} // end of namespace pni 

#endif // pniplatform_h


