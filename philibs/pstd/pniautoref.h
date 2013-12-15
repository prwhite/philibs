/////////////////////////////////////////////////////////////////////
//
//	class: autoRef
//
//	Templatized Auto Reference class for refCount classes
//
/////////////////////////////////////////////////////////////////////

#ifndef pniautoref_h
#define pniautoref_h

/////////////////////////////////////////////////////////////////////

namespace pni {
	namespace pstd {

/////////////////////////////////////////////////////////////////////

template< class Type >
class autoRef
{
	public:
		//typedef typename Type value_type;
		typedef Type value_type;

		// Ref'ed first, the assigned: this avoids aliasing problems
		autoRef ( value_type* refClassIn = 0 )
		{
			if ( refClassIn )
				refClassIn->ref ();

			refClass = refClassIn;
		}

		// Ref'ed first, the assigned: this avoids aliasing problems
		autoRef ( autoRef const& rhs )
		{
			if ( rhs.refClass )
				rhs.refClass->ref ();
			
			refClass = rhs.refClass;
		}

		~autoRef ()
		{
			if ( refClass )
				refClass->unref ();
			refClass = 0; // Otherwise operator bool() can fail
		}

		autoRef& operator = ( value_type* rhs )
		{
			if ( rhs )
				rhs->ref (); 
		 	if ( refClass )
				refClass->unref ();
			refClass = rhs;
			return *this;
		}

		autoRef& operator = ( autoRef const& rhs )
		{
			if ( rhs.refClass )
				rhs.refClass->ref (); 
			if ( refClass )
				refClass->unref ();
			refClass = rhs.refClass;
			return *this;
		}
		
		value_type& operator * () const { return *refClass; }
		value_type* operator -> () const { return refClass; }
		bool operator < ( value_type const* rhs ) const { return refClass < rhs; }
		bool operator < ( autoRef const& rhs ) const { return refClass < rhs.refClass; }
		bool operator == ( value_type const* rhs ) const { return refClass == rhs; }
		bool operator == ( autoRef const& rhs ) const { return refClass == rhs.refClass; }

		operator bool () const { return refClass != 0; }

		value_type* get () const { return refClass; }

	protected:
		value_type* refClass;
		
	private:

};

/////////////////////////////////////////////////////////////////////

	} // end of namespace pstd 
} // end of namespace pni 

#endif // pniautoref_h


