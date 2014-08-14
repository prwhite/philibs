

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>


using namespace std;

using Byte = uint8_t;
static size_t const Stride = 3;

namespace {

inline float nfrand () { return ( rand () % 255 ) / 255.0f; }
inline float vf3len ( float xx, float yy, float zz ) { return sqrtf ( xx * xx + yy * yy + zz * zz ); }

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
      float rf = nfrand();
      float gf = nfrand();
      float bf = nfrand();

        // normalize vector
      float len = vf3len ( rf, gf, bf );
      rf /= len;
      gf /= len;
      bf /= len;

        // fill data in output array, putting in byte range [0,255]
      Byte* dst = &bytes[ ( yy * dim + xx ) * Stride ];
      dst[ 0 ] = rf * 0xff;
      dst[ 1 ] = gf * 0xff;
      dst[ 2 ] = bf * 0xff;
    }
  }

  ostringstream ostr;
  ostr << "rand-" << dim << ".data";

  ofstream out ( ostr.str () );
  out.write ( (char const* ) bytes.data(), bytes.size() );

  cout << "wrote file " << ostr.str () << endl;
}


