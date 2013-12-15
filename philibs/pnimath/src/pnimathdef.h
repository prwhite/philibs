
#ifndef pnimathdef_h
#define pnimathdef_h


//#ifdef WIN32
//	#ifdef PNIMATHMAKELIB
//
//		#define PNIMATHAPI __declspec( dllexport )
//
//	#else
//
//		#define PNIMATHAPI __declspec( dllimport )
//
//	#endif
//
//#else
	
	#define PNIMATHAPI

//#endif



// Auto-link

//#ifndef PNIMATHMAKELIB
//
//	#ifdef WIN32
//		#ifdef DEBUG
//			#pragma message( "Automatically linking with pnimath_d.lib" )
//			#pragma comment( lib, "pnimath_d" )
//		#else
//			#pragma message( "Automatically linking with pnimath.lib" )
//			#pragma comment( lib, "pnimath" )			
//		#endif
//	#endif
//
//#endif


#endif // pnimathdef_h
