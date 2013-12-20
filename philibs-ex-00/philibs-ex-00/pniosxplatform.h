/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

#ifndef pniosxplatform_h
#define pniosxplatform_h

/////////////////////////////////////////////////////////////////////


#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string>

enum ShellLoc
{
  DocsDir,    // AppDir/Documents - persistent and backed up
  AppDir,     // AppDir
  BundleDir,	// AppDir/AppName.app - don't write anything here!
  TmpDir      // AppDir/tmp - wiped periodically  
};

std::string nsStringToStdString ( NSString* pStr )
{
  const size_t MaxSize = 1024;
  char str[ MaxSize ] = { 0 };
  
  [pStr getCString:str maxLength:MaxSize encoding:NSASCIIStringEncoding];
  
  return std::string ( str );
}

/** Get the system's path to a given directory.
 Includes:
   DocsDir, AppDir, BundleDir, TmpDir locations.
 @param loc The ShellLoc location enum specifying the desired location.
*/
 
std::string getShellPath ( ShellLoc loc )
{
  std::string retVal;
  
  switch ( loc )
  {
    case DocsDir:
    {
      NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
      NSString *documentsDirectory = [paths objectAtIndex:0];
      retVal = nsStringToStdString(documentsDirectory);
    }
      break;
    case AppDir:
    {
      retVal = nsStringToStdString ( NSHomeDirectory () );
    }
      break;
    case BundleDir:
    {
      //      NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
      //      NSString *bundleDirectory = [paths objectAtIndex:0];
      retVal = nsStringToStdString ( [[NSBundle mainBundle] bundlePath] );
    }
      break;
    case TmpDir:
    {
      retVal = nsStringToStdString ( NSTemporaryDirectory() );
    }
      break;
    default:
      break;
  }
  
  return retVal;
}

void testLs ( std::string const& str )
{
  printf ( "dir listing of %s\n", str.c_str () );
  DIR* dir = opendir ( str.c_str () );
  while ( dirent* dent = readdir(dir) )
  {
    printf ( "  %s\n", dent->d_name );
  }
}

void testShellDir ( std::string const& msg, ShellLoc loc )
{
  std::string dir ( getShellPath ( loc ) );
  printf("%s dir = %s\n", msg.c_str (), dir.c_str() );
  testLs ( dir );
}

void testFileSys ()
{
  testShellDir ( "doc", DocsDir );
  testShellDir ( "tmp", TmpDir );
  testShellDir ( "app", AppDir );
  testShellDir ( "bundle", BundleDir );
  
  size_t MaxPathSize = 1024;
  char cwd[ MaxPathSize ];
  printf("pwd = %s\n", getcwd(cwd, MaxPathSize) );
}

/////////////////////////////////////////////////////////////////////

#endif // pniosxplatform_h

