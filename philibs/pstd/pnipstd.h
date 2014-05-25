// ///////////////////////////////////////////////////////////////////
//
//  misc: pstd
//
// ///////////////////////////////////////////////////////////////////

#ifndef pnipstd_h
#define pnipstd_h

#include <functional>

// ///////////////////////////////////////////////////////////////////



// ///////////////////////////////////////////////////////////////////

// supplement std namespace

  // from http://stackoverflow.com/questions/7222143/unordered-map-hash-function-c

namespace std
{
  template <class T>
  inline void hash_combine(std::size_t & seed, const T & v)
  {
    hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  template<typename S, typename T> struct hash<pair<S, T>>
  {
    inline size_t operator()(const pair<S, T> & v) const
    {
      size_t seed = 0;
      hash_combine(seed, v.first);
      hash_combine(seed, v.second);
      return seed;
    }
  };
}

// ///////////////////////////////////////////////////////////////////

namespace pni {
  namespace pstd {
    
// ///////////////////////////////////////////////////////////////////


// ///////////////////////////////////////////////////////////////////

  } // end namespace pstd
} // end of namespace pni 

#endif // pnipstd_h
