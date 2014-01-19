/////////////////////////////////////////////////////////////////////
//
//    file: imgbase.cpp
//
/////////////////////////////////////////////////////////////////////

#include "imgbase.h"

#include <fstream>
#include <iostream>

#include "pnirefcountdbg.h"

/////////////////////////////////////////////////////////////////////

namespace img {
  
/////////////////////////////////////////////////////////////////////

// base::base(base const& rhs)
// {
//   
// }
// 
// base& base::operator=(base const& rhs)
// {
//   
//   return *this;
// }
// 
// bool base::operator==(base const& rhs) const
// {
//   
//   return false;
// }

/////////////////////////////////////////////////////////////////////

void base::alloc ( bool mipMapsToo )
{
  size_t const Stride = calcFormatStride ( mFormat );
  
  mBuffers.resize ( 0 );
  
  bool done = false;
  size_t pitch = mPitch;
  size_t height = mHeight;
  
  while ( ! done )
  {
    mBuffers.push_back ( new buffer );
    mBuffers.back ()->resize ( pitch * height );
    
    if ( ! mipMapsToo ) done = true;
    
    if ( height == 1 && pitch == Stride ) done = true;
    
    height /= 2;
    pitch /= 2;
    
    if ( height < 1 )
      height = 1;
    
    if ( pitch < 1 )
      pitch = 1;
  }
}

/////////////////////////////////////////////////////////////////////

void base::cloneFormatAndSize ( base const* pBase, bool reallocToo )
{
  if ( mFormat == pBase->mFormat &&
      mWidth == pBase->mWidth &&
      mHeight == pBase->mHeight &&
      mPitch == pBase->mPitch )
  {
    
  }
  else
  {
    mBuffers.resize ( 0 );  // Throw away old buffers, yo.
    mFormat = pBase->mFormat;
    mWidth = pBase->mWidth;
    mHeight = pBase->mHeight;
    mPitch = pBase->mPitch;
  }

  if ( reallocToo )
  {
    size_t newSize = pBase->mBuffers.size ();

    mBuffers.resize ( 0 );  // Throw away old buffers, yo.
    mBuffers.resize ( newSize );
  
    for ( size_t num = 0; num < newSize; ++num )
    {
      mBuffers[ num ] = new buffer;
      mBuffers[ num ]->resize ( pBase->mBuffers[ num ]->size () );
    }
  }
}

/////////////////////////////////////////////////////////////////////

void base::uniquifyBuffers ()
{
  Buffers tmp;

  Buffers::iterator end = mBuffers.end ();
  for ( Buffers::iterator cur = mBuffers.begin ();
      cur != end;
      ++cur )
  {
    tmp.push_back ( new buffer ( *cur->get () ) );
  }
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

namespace
{
  typedef uint32_t IntType;
  
  void writei ( std::ostream& str, IntType val )
  {
    str.write ( ( char* ) &val, sizeof ( IntType ) );
  }
  
  IntType readi ( std::istream& str )
  {
    IntType tmp;
    str.read ( ( char* ) &tmp, sizeof ( IntType ) );
    return tmp;
  }
}

/////////////////////////////////////////////////////////////////////

bool base::read ( std::string const& fname )
{
  setName ( fname );
  
  std::ifstream in ( fname.c_str (), 
      std::ios_base::in | std::ios_base::binary );

  if ( in.good () && in.is_open () )
  {
    Dim width = readi ( in );
    Dim height = readi ( in );
    Dim pitch = readi ( in );
    
    mWidth = width;
    mHeight = height;
    mPitch = pitch;
    
    mFormat = ( Format ) readi ( in );
    size_t size = ( size_t ) readi ( in );

    alloc ( size > 1 );

    for ( size_t num = 0; num < size; ++num )
    {
      BufferType* pData = &( ( *mBuffers[ num ] )[ 0 ] );

      in.read ( reinterpret_cast< char* > ( pData ), mBuffers[ num ]->size () );  
    }
    
    return true;
  }
  else  
    return false;
}

/////////////////////////////////////////////////////////////////////

bool base::write ( std::string const& fname )
{
  setName ( fname );
  
  std::ofstream out ( fname.c_str (), 
      std::ios_base::out | std::ios_base::trunc | std::ios_base::binary );

  if ( out.good () && out.is_open () )
  {
    writei ( out, ( int ) mWidth );
    writei ( out, ( int ) mHeight );
    writei ( out, ( int ) mPitch );
    writei ( out, ( int ) mFormat );
    writei ( out, ( int ) mBuffers.size () );

    for ( size_t num = 0; num < mBuffers.size (); ++num )
    {
      BufferType* pData = &( ( *mBuffers[ num ] )[ 0 ] );

      out.write ( reinterpret_cast< char* > ( pData ), mBuffers[ num ]->size () );  
    }
    
    return true;
  }
  else
    return false;
}

/////////////////////////////////////////////////////////////////////

void base::writeRaw ( std::string const& fname )
{
  std::ofstream out ( fname.c_str (), 
      std::ios_base::out | std::ios_base::binary );

  BufferType* pData = &( ( *mBuffers[ 0 ] )[ 0 ] );
  out.write ( reinterpret_cast< char* > ( pData ), mBuffers[ 0 ]->size () );  
  
    // Write out the data 2 more times for grayscale so we can read
    // it with gimps raw importer for planar rgb.
  if ( mFormat == Gray8 )
  {
    out.write ( reinterpret_cast< char* > ( pData ), mBuffers[ 0 ]->size () );  
    out.write ( reinterpret_cast< char* > ( pData ), mBuffers[ 0 ]->size () );  
  }
}

/////////////////////////////////////////////////////////////////////
// overrides from pni::pstd::refCount

void base::onDoDbg ()
{
  std::ostream* pStr = pni::pstd::dbgRefCount::getStr ();
  
  *pStr << " file = " << mName;
}

void base::collectRefs ( pni::pstd::refCount::Refs& refs )
{
  pni::pstd::dbgRefCount::collectVecRefs ( mBuffers, refs );
}

/////////////////////////////////////////////////////////////////////


} // end of namespace img 


