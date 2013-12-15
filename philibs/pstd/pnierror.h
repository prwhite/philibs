/////////////////////////////////////////////////////////////////////
//
//	class: error
//
//	This class represents an error and should be passed with
//	any event that is notifying someone of an error.
//
/////////////////////////////////////////////////////////////////////


#ifndef pnierror_h
#define pnierror_h

/////////////////////////////////////////////////////////////////////

// #include "pnipstddef.h"
// 
// #include "pnipstd.h"

#include <string>

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {

    typedef int int32;

/////////////////////////////////////////////////////////////////////

class error 
{
	public:
		error ( pstd::int32 errorCode, std::string const& errorString ) :
			string ( errorString ),
			code ( errorCode )
		{}
//		error ( error const& rhs ) :
//			code ( rhs.code ),
//			string ( rhs.string )
//		{}
		virtual ~error () {}
//		error& operator = ( error const& rhs )
//		{ 
//			code = rhs.code;
//			string = rhs.string; 
//			return *this; 
//		}

		std::string const& getString () const { return string; }
		pstd::int32 getCode () const { return code; }

	protected:

	private:
		std::string string;
		pstd::int32 code;
};

/////////////////////////////////////////////////////////////////////

	} // end of namespace pstd
} // end of namespace pni 

#endif // pnierror_h


