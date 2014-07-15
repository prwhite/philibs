// ///////////////////////////////////////////////////////////////////
//
//  class: typetraits
//
// ///////////////////////////////////////////////////////////////////

#ifndef pnitypetraits_h
#define pnitypetraits_h

// ///////////////////////////////////////////////////////////////////

// ///////////////////////////////////////////////////////////////////

namespace pni {
  namespace pstd {


  } // end of namespace pstd
} // end of namespace pni

// ///////////////////////////////////////////////////////////////////

#include <type_traits>
#include <iostream>

// ///////////////////////////////////////////////////////////////////

  // from http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Member_Detector

#define GENERATE_HAS_MEMBER(member)                                               \
                                                                                  \
template < class T >                                                              \
class HasMember_##member                                                          \
{                                                                                 \
private:                                                                          \
    using Yes = char[2];                                                          \
    using  No = char[1];                                                          \
                                                                                  \
    struct Fallback { int member; };                                              \
    struct Derived : T, Fallback { };                                             \
                                                                                  \
    template < class U >                                                          \
    static No& test ( decltype(U::member)* );                                     \
    template < typename U >                                                       \
    static Yes& test (...);                                                       \
                                                                                  \
public:                                                                           \
    static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes); \
};                                                                                \
                                                                                  \
template < class T >                                                              \
struct has_member_##member                                                        \
: public std::integral_constant<bool, HasMember_##member<T>::RESULT>              \
{ };

// ///////////////////////////////////////////////////////////////////


#endif // pnitypetraits_h
