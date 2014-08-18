

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>


using namespace std;

using Byte = uint8_t;
static size_t const Stride = 3;

namespace {

using Vec = float[3];

inline float nfrand () { return ( rand () % 255 ) / 255.0f; }
inline void vnfrand ( Vec vec ) { vec[0] = nfrand(); vec[1] = nfrand(); vec[2] = nfrand(); }
inline float vf3len ( Vec vec ) { return sqrtf ( vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2] ); }
inline void vf3normalize ( Vec vec ) { float len = vf3len(vec); vec[0]/=len; vec[1]/=len; vec[2]/=len; }
inline void vf3mult ( Vec vec, float val ) { vec[0] *= val; vec[1] *= val; vec[2] *= val; }
inline void vf3add ( Vec vec, float val ) { vec[0] += val; vec[1] += val; vec[2] += val; }
inline void vf3dbg ( Vec vec ) { cout << "vec " << vec << " = " << vec[0] << ", " << vec[1] << ", " << vec[2] << endl; }
inline void vf3copy ( Vec const src, Vec dst ) { dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; }
inline void vf3assign ( Vec const src, Byte* dst ) { dst[0]=(int)(src[0]*0xff)&0xff, dst[1]=(int)(src[1]*0xff)&0xff, dst[2]=(int)(src[2]*0xff)&0xff; }
}

int main ( int argc, char* argv[] )
{
  size_t dim = 64;
  if ( argc == 2 )
    dim = atoi ( argv[ 1 ] );

  cout << "using dimension " << dim << endl;

  srand ( dim );

  using Bytes = vector< Byte >;
  Bytes bytes;
  bytes.resize ( dim * dim * Stride );

  for ( size_t yy = 0; yy < dim; ++yy )
  {
    for ( size_t xx = 0; xx < dim; ++xx )
    {
        // generate vector of normalized floats /componenents/
      Vec vec;

      vnfrand ( vec );    // (0,1)
      vf3mult ( vec, 2.0f ); // (0,2)
      vf3add ( vec, -1.0f ); // (-1,1)
      vf3normalize ( vec );  // [-1,1]
      vf3add ( vec, 1.0f );
      vf3mult ( vec, 0.5f );

        // fill data in output array, putting in byte range [0,255]
      Byte* dst = &bytes[ ( yy * dim + xx ) * Stride ];
      vf3assign ( vec, dst );
    }
  }

  ostringstream ostr;
  ostr << "rand-" << dim << ".data";

  ofstream out ( ostr.str () );
  out.write ( (char const* ) bytes.data(), bytes.size() );

  cout << "wrote file " << ostr.str () << endl;
}


